
#if 0

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
        
        regions.push_back(new Region( 0,HEIGHT,   0,   0,   0,REGION_MOVIE,DEFAULTFOLDER "/day00/The_black_hole_vertical_v001.mov"));
        hole = regions.back();
        hole->play();
        //    hole->movie_stop();
        
        /*
         regions.push_back(new Region( 0,     0,   0,   0,   0,REGION_MOVIE, DEFAULTFOLDER "/s06_comp_V05_h264.mov"));
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

#endif
