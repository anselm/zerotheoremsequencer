
#define REGION_EMPTY 0
#define REGION_IMAGE 10
#define REGION_REMAP 20
#define REGION_CUBES 30
#define REGION_MOVIE 40
#define REGION_QUEUE 50
#define REGION_EVENT 60
#define REGION_VIDEO 70
#define REGION_DECKV 71
#define REGION_WORDS 80
#define REGION_STREAM 90
#define REGION_MASK 100
#define REGION_FPS 110

// regions
class Region {
public:
    int x,y,w,h;                    // a region has a position
    int ntextures;                  // a region may be render an animation from separate tiffs
    int kind;                       // indicates if we want to remap this region or other kinds of region types
    string filename;                // a region may have data from this source (a movie or folder of individual tiffs)
    int targetx,targety,targetw,targeth,targetease;
    float targetspeed;
    
    qtime::MovieGl movie;      // pointer to movie if any
    int clickable = 0;              // clickable
    int nframes = 0;
    int visible = 1;
    int looping = 0;
    int loopcount = 0;
    int playing = 0;
    float cframe = 0;
    float rate = 1;
    int dirty = 0;
    int range_low = 0;
    int range_high = 1;
    float z = 0;                    // sort order
    int loaded = 0;
    int done = 0;
    float rotate = -90;
    
    // sequences of textures
    vector<gl::Texture> textures;
    vector<int> textureoffsets;
    int texindex,texlow,texhigh;
    int scroll = 0;
    int sequencebitlen = 0;
    int stream_images = 0;
    int stream_cubeface = 0;
    int stream_tile = 0;
    float speed = 5;
    float speed_target = 5;
    float speed_dir = 1;
    int tw,th;
    TriMesh mesh;
    
    // remapping shaders
    gl::GlslProg shader;
    gl::GlslProg shader2;
    gl::Texture tex0;
    gl::Texture tex1;
    float fader = 1.0;
    float fader_target = 0.0;
    int facelock[6];
    
    // video
    Capture				mCapture;
    Surface             mSurface;
    gl::Texture			mTexture;
    int                 mValid = 0;
    
    gl::Fbo fbo;
    float fbopos = 0;
    
    // text scroll
    vector<string> terms;
    float zoom = 0;

    // decklink video
    DeckLinkController* deckvideo;
    Surface decksurface;

    Region(int _x,int _y,int _w,int _h,int _ntextures,int _kind,string _filename);

    void load_queue();

    void update(float ratio);
    void draw();

    void reset() {
        dirty = 1;
        visible = 1;
        playing = 1;
        loopcount = 0;
        looping = 0;
        cframe = 0;
        range_low = 0;
        range_high = nframes;
        done = 0;
        rate = 1;
        targetx = targety = targetw = targeth = targetspeed = targetease = 0;
        if(movie) {
            w = movie.getWidth();
            h = movie.getHeight();
            nframes = movie.getNumFrames();
        }
    }

    void restart() {
        visible = playing = 1;
        cframe = 0;
        done = 0;
        //movie.seekToFrame(0);
    }

    void hide() {
        visible = 0;
        playing = 0;
    }

    void stop() {
        playing = 0;
    }

    void play() {
        playing = 1;
        visible = 1;
        done = 0;
    }
    
    inline float signof(float val) { return (0.0 < val) - (val < 0.0); }

    void targetstep(float amount) {
        if(!targetspeed) return;
        if(targetease) {
            x += (targetx-x)/2.0f * amount * targetspeed;
            y += (targety-y)/2.0f * amount * targetspeed;
            w += (targetw-w)/2.0f * amount * targetspeed;
            h += (targeth-h)/2.0f * amount * targetspeed;
        } else {
            float temp = signof(targetx-x);
            console() << "blah" << temp << endl;
            x += signof(targetx-x) * amount * targetspeed;
            y += signof(targety-y) * amount * targetspeed;
            w += signof(targetw-w) * amount * targetspeed;
            h += signof(targeth-h) * amount * targetspeed;
        }
    }
    
    void step(float amount) {

        if(targetspeed)targetstep(amount);
        
        float lframe = cframe;

        cframe += amount;
        
        // if(amount > (range_high-range_low)) amount = amount%(range_high-range_low);
        // if(amount < -(range_high-range_low)) amount = (range_high-range_low)-(amount%(range_high-range_low));
        
        if(cframe >= range_high) {
            if(looping) {
                if(loopcount) { loopcount--; if(loopcount <= 0) looping = 0; }
                while(cframe > range_high) cframe -= (range_high-range_low);
            } else {
                cframe = range_high - 1;
                done = 1;
            }
        }
        else if(cframe < range_low) {
            if(looping) {
                if(loopcount) { loopcount--; if(loopcount <= 0) looping = 0; }
                while(cframe < range_low) cframe += (range_high-range_low);
            } else {
                cframe = range_low;
                //done = 1; disable this backwards case because we are often before the start
            }
        }

        if(movie) {
            if(1) {
                if(((int)cframe) != ((int)lframe) || dirty) { movie.seekToFrame(cframe); dirty = 1; }
            } else {
                // another way is to do this is to step it - but this fails to deal with wrap around
                while(((int)lframe) > ((int)cframe)) {
                    movie.stepBackward();
                    lframe--;
                    dirty = 1;
                }
                while(((int)lframe) < ((int)cframe)) {
                    movie.stepForward();
                    lframe++;
                    dirty = 1;
                }
            }
        }
    }
};

extern vector<class Region*> regions;

