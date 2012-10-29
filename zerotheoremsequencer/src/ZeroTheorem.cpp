
#include "ZeroTheorem.h"
#include "Region.h"
#include "Deck.h"
#include "Script.h"

#include "cinder/CinderResources.h"

#define RES_VERTS CINDER_RESOURCE( ../, verts.glsl, 128, GLSL )
#define RES_CUBE  CINDER_RESOURCE( ../, cube.glsl, 129, GLSL )
#define RES_CUBE2 CINDER_RESOURCE( ../, cube2.glsl, 129, GLSL )

#define FRAMERATE_IDEAL 25.0f
#define FRAMERATE_MEDIAN 1.0f

float framerate = 0.0f;
float frame_median_rate = FRAMERATE_MEDIAN;
float frame_ideal_speed = FRAMERATE_MEDIAN/FRAMERATE_IDEAL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Group Black Hole
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GroupBlackHole: public Group {
public:
    Region* hole;
    Region* anon;
    int mode = 1;

    void setMode(int mode) {
        switch(mode) {
            case 0:
            case 1:
                if(1) {
                    hole->restart();
                    anon->hide();
                }
                break;

            case 2:
                if(1) {
                    hole->hide();
                    anon->reset();
                    anon->play();
                }
                break;

            case 3:
                if(1) {
                    hole->hide();
                    anon->restart();
                    anon->movie.seekToFrame( anon->movie.getNumFrames()-3);
                    anon->cframe = anon->movie.getNumFrames()-3;
                }
                break;
        }
    }

    
    void update() {
        // console() << "anon is at " << anon->movie.getFramerate() << " " << anon->movie.getCurrentTime() << endl;
        // hole->movie.setRate(frame_median_rate);
        // anon->movie.setRate(frame_median_rate);
    }

    void setup() {

        regions.push_back(new Region( 0,HEIGHT,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/day00/The_black_hole_vertical_v001.mov"));
        hole = regions.back();
        hole->play();
    //    hole->movie_stop();

/*
        regions.push_back(new Region( 0,     0,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/s06_comp_V05_h264.mov"));
        anon = regions.back();
        anon->movie.setLoop( false, false );
//        anon->movie.play();
        anon->rotate = 0;
        anon->visible = 0;
*/
        //regions.push_back(new Region(  400, 400, DECKWIDTH/DECKRATIO, DECKHEIGHT/DECKRATIO,   0,REGION_DECKV,""));

       // regions.push_back(new Region(  100, 200, 200, 75,   0,REGION_FPS,""));

       // regions.push_back(new Region(  355,   0,   0,1080,   0,REGION_WORDS, "Anselm's test of  multiple full screen large videos and a test of the blackmagic playback in real time as an overlay "));
    }

    void mouseDown(MouseEvent event) {
        mode++;
        if(mode>3) mode = 1;
        setMode(mode);
    }

    void keyDown( KeyEvent event ) {
        switch(event.getChar()) {
            case '1': setMode(1); break;
            case '2': setMode(2); break;
            case '3': setMode(3); break;
            default: break;
        }
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cinder app helper class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class QTimeAdvApp : public AppBasic {
public:

    Group* currentGroup;
    Settings* settings;
    Timer timer;

	void prepareSettings( Settings *settings ) {
        this->settings = settings;
        settings->setWindowSize( WIDTH, HEIGHT );
        settings->setFullScreen( false );
        settings->setResizable( true );
        //gl::enableVerticalSync();
        settings->setFrameRate(25);
        // glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
        //glDisable(GL_MULTISAMPLE);
        timer.start();
    }

    void setup() {
        // currentGroup = new GroupCube();
        //currentGroup = new GroupBlackHole();
        currentGroup = new Script();
        currentGroup->setup();
    }

    int m = 1;
    
	void keyDown( KeyEvent event ) {

        switch(event.getChar()) {
            case 'q': setFullScreen(0); showCursor(); quit(); return;
            case 'f': setFullScreen( ! isFullScreen() ); return;
            case 'm': m = 1 - m; if(!m) hideCursor(); else showCursor(); return;
            default: break;
        }

        switch(event.getCode()) {
            case KeyEvent::KEY_RIGHT:  glTranslatef(1.0,0.0,0.0); return;
            case KeyEvent::KEY_LEFT:   glTranslatef(-1.0,0.0,0.0); return;
            case KeyEvent::KEY_UP:    glTranslatef(0.0,-1.0,0.0); return;
            case KeyEvent::KEY_DOWN:  glTranslatef(0.0,1.0,0.0); return;
            case 269: glTranslatef(0.0,0.0,-1.1); return;
            case 61: glTranslatef(0.0,0.0,1.1); return;
            default: break;
        }

        currentGroup->keyDown(event);
    }
    
    int updated = 0;

	void update() {
        updated++;
        
        // stop the timer and see if we have wasted enough time yet; else waste some time
        timer.stop();
        float time = timer.getSeconds();
        
        // if general performance was better than our budget then waste the excess budget
        if(time < frame_ideal_speed && time >=0) {
            sleep( ( frame_ideal_speed - time ) * 1000.0f );
            //console() << " killing some time " << (time_per_frame - time) << endl;
        }
        timer.start();

        // if general performance was poor then try advance by this multiplier of the time we failed to make up
        // so for example if it took us 1 second and we wanted 0.5 seconds we want to pass a multiplier of 2...
        float ratio = 1.0;
        if(time > frame_ideal_speed && time < frame_ideal_speed*10) {
            ratio = time / frame_ideal_speed;
        }
        
        // if general performance was worse than our budget then skip ahead
       // for(float i = 0; i < time; i+= frame_ideal_speed) {
            currentGroup->update();
            for( size_t m = 0; m < regions.size(); ++m ) {
                regions[m]->update(ratio);
            }
       // }
    }

	void draw() {
        if(!updated) {
            console() << "drew before updated!" << endl;
            return;
        }
        gl::clear();
        for( size_t m = 0; m < regions.size(); ++m ) {
            regions[m]->draw();
        }
        framerate = this->getAverageFps();
    }

    void mouseMove(MouseEvent event) {
/*
        // use mouse to set framerate
        float rate = ( event.getY() * 400.0f / ( HEIGHT * 1.0f ) );
        if(rate > 400 ) rate = 400;
        if(rate < 25 ) rate = 25;
        frame_median_rate = rate / 30.0f; // 30fps is an arbitrary median
        frame_ideal_speed = 1.0f / rate;
        console() << "1 setting frame rate " << frame_ideal_speed << endl;
*/
    }
    
    void mouseDown (MouseEvent event) {
        currentGroup->mouseDown(event);
    }
    
    void mouseUp (MouseEvent event) {
        currentGroup->mouseUp(event);
    }

};


CINDER_APP_BASIC( QTimeAdvApp, RendererGl(0) )


/*
 
TODO

 - the remote viewer needs the ability to do things on command; wait, play something specific etc
    i think i can modify a file from libcinder
    and have it look at that as a web page
 
 - i need a real reset

 - script detect changes
 

INTERACTIVE CUBE    
 
 speed
   - switch to 64 bit cinder for speed

 cube
 - inject real faces at right times
 
 mouse
 - having a mouse that moves to the positions
 
 conveyer belt
 - make the slidy things slide across and go down for a conveyer belt
 
 streams and ribbons
 
 <- have a box on the stream for picking
 - have stream be a bit jerky in general; more like film
 - grey strip at edge of selected
 - see about using zoomed versions of the stream images for the ribbonized view rather than small
 - stream should smoothly expand and shrink
 - stream shader should be more ribbony
 - often there are two stream tiles in a row why?
 
 mouth
 - i gather the mouth is supposed to bounce around - also get which mouth animations to play when
 
 small issues to check
 
 - newer bottom codes? and the bottom codes are supposed to be paused at different points - when?
 - there are other variations of layouts?
 - randomize picking order and avoid dupes or let terry set this; also allow first one to be a dupe at the very end of course
 - let terry or somebody pick exactly which images to put on cube also
 - empower other people to pick which streams to show
 - get exact placement of all movies
 - look to smooth or antialias the texture i multiply against the backdrop on the cube
 - [low priority] remap cache could buffer surfaces so that we don't thrash disk as much? ( or can it? possibly not )
 
 */
