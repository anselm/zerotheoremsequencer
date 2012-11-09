
#include "ZeroTheorem.h"
#include "Region.h"
#include "Deck.h"
#include "Cube.h"

#define CUBE_REMAP_FILES "/zerotheoremshared/cube2/remap/TRIANG_e_REMAP.%d.png"
#define CUBE_CUBES_FILES "/zerotheoremshared/cube2/cube/TRIANG_e_680_%03d.jpg"
#define CUBE_WHITE_FILES "/zerotheoremshared/cube2/white/TRIANG_e_WHITE_680_%03d.jpg"

/*
 int timing[] = {
 
 // face a1
 
 1, 120, // mancom logo 5
 121, 168, // random 3  ( tristripe scrambly 121, 121+24, 121+24+24 )
 169,1000, // random hold
 1001,1360, // random 15 (tristripe scrambly )
 1361,1600, // end hold
 
 // A2
 
 1,   8, // mancom logo
 9, 128, // mancom logo 5
 129, 176, // random tristripe scrambly thing 2
 177,1008, // random hold
 1009,1368, // random tristripe scrambly thing 15
 1369,1600, // SOLVED B
 
 // A3
 
 1,  16,  // mancom logo
 17, 136,  // mancom logo 5
 137, 232,  // random 4
 233,1016,  // SOLVED A
 1017,1352,  // random 14
 1353,1600,  // random hold
 
 // B1
 
 1, 144,  // mancom logo 6
 145, 408,  // random 11
 409,1600,  // SOLVED
 
 // B2
 
 1,   8,  // mancom logo
 9, 152,  // mancom logo 6
 153, 248,  // random 4
 249,1600,  // random hold
 
 // B3
 
 1,  16,  // mancom logo
 17, 136,  // mancom logo 5
 137, 472,  // random 14
 473,1600,  // random hold
 
 // c1
 
 1, 120   // mancom logo 5
 };
 */


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Another group of related assets
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CubeGroup: public Group {
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // init and reset
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    int mode = 0;
    int picked = 0;
    int expectingdown = 0;
    int mousedown = 0;
    Region* stream = 0;
    Region* cube = 0;
    Region* mouth = 0;
    Region* streams[6] = { 0,0,0,0,0,0 };
    
    void setup () {
        
        // ************************************* load pretty cube art for each frame; blending two groups into one

        cacheLoadAll(CUBE_WHITE_FILES,1,CUBE_WHITE,CUBE_END);
        cacheLoadAll(CUBE_CUBES_FILES,1,CUBE_START,CUBE_WHITE);
 
        // ************************************* build regions

         regions.push_back(new Region(   0,   0,1920,1080,   0,REGION_IMAGE,"/zerotheoremshared/bottom.jpg"));
         
         // the cube data on disk is index1
         regions.push_back(new Region(1050, 250, 512, 512,CUBE_END,REGION_REMAP,CUBE_REMAP_FILES));
         cube = regions.back();
        cube->texlow = CUBE_START;
        cube->texhigh = CUBE_MANCOM;
         cube->startThreadLoad();

         regions.push_back(new Region( 355,1080,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/mancom_bottom_TypeBall.mov"));
         regions.back()->looping = 1;
         regions.push_back(new Region(500, 1090,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/Interface_E-LeftSideCodes.mov"));
         regions.back()->looping = 1;
         regions.push_back(new Region(1680, 860,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/Interface_E_BottomCode.mov"));
         regions.back()->looping = 1;
         
         // regions.push_back(new Region( 217, 740,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/from_boris/mouth_anoying/mouth_anoying_tst_A/quicktime_animation_uncompress_audio/mouthb.mov"));
         

        regions.push_back(new Region(  55,   0,   0,1080,   0,REGION_WORDS,
                                     "YOUR PEERS ARE WORKING HARDER THAN YOU "
                                     "DO YOU FEEL YOU ARE GIVING IT YOUR ALL "
                                     "REWARDS FOR BETTER PERFORMANCE "
                                     "WORK HARDER CLICK FASTER"));
        regions.back()->speed = 15;
        
        /*
         // the streams
         regions.push_back(new Region( 482+53*0,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[0] = regions.back();
         
         regions.push_back(new Region( 482+53*1,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[1] = regions.back();
         
         regions.push_back(new Region( 482+53*2,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[2] = regions.back();
         
         regions.push_back(new Region( 482+53*3,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[3] = regions.back();
         
         regions.push_back(new Region( 482+53*4,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[4] = regions.back();
         
         regions.push_back(new Region( 482+53*5,  20,   48*3,756,  33,REGION_STREAM,"/zerotheoremshared/streams/small/%d/%d.png"));
         streams[5] = regions.back();
         
         regions.push_back(new Region(  467, 790,   0,   0,   0,REGION_MOVIE,"/zerotheoremshared/Mouth_Framed_Scene_23_v02b.mov"));
         mouth = regions.back();
         regions.back()->playloop();
         
         //        regions.push_back(new Region(  400, 400, 960, 720,   0,REGION_VIDEO,""));
         
         */
        
        //regions.push_back(new Region(  400, 400, DECKWIDTH/DECKRATIO, DECKHEIGHT/DECKRATIO,   0,REGION_DECKV,""));
        
        
        regions.push_back(new Region(  100, 100, 200, 100,   0,REGION_FPS,""));
        
        
        // ************************************* reset and go
        
        modeReset();
        
        // do the load late because some properties are not set till now
        for(int i = 0; i<6;i++) {
            if(streams[i] && !streams[i]->loaded) { streams[i]->load_queue(); streams[i]->loaded = 1; }
            break;
        }
        
    }
    
    void loadManComLogos() {
        for(int i = 0; i < 6;i++) {
            char blah[512];
            sprintf(blah,"/zerotheoremshared/cube2/TRIANG_logos/mancom_0%d.jpg",i+1);
            try {
                gl::Texture tex = Surface( loadImage(blah));
                if(cube->textures.size() > i*3+2) {
                    cube->textures[i*3+0] = tex;
                    cube->textures[i*3+1] = tex;
                    cube->textures[i*3+2] = tex;
                } else {
                    cube->textures.push_back(tex);
                    cube->textures.push_back(tex);
                    cube->textures.push_back(tex);
                }
                console() << "loaded mancom logo " << blah << std::endl;
            } catch(...) {
                console() << "failed to load mancom data ! : " << blah << std::endl;
                exit(0);
            }
        }
    }
    
    
    void modeReset() {
        
        picked = 0;
        mode = 0;
        mousedown = 0;
        stream = 0;
        expectingdown = 0;
        
        if(mouth) {
            mouth->visible = 0;
            mouth->stop();
        }
        
        if(cube) {
            cube->texindex = 0;
            cube->loaded = 0;
            cube->fader = cube->fader_target = 1.0;
            cube->facelock[0]=cube->facelock[1]=cube->facelock[2]=cube->facelock[3]=cube->facelock[4]=cube->facelock[5]=1;
            loadManComLogos();
        }
        
        stream = streams[0];
        if(stream) {
            stream->speed = stream->speed_target = 33;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = 1;
            stream->clickable = 1;
            stream->stream_images = 0;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        stream = streams[1];
        if(stream) {
            stream->speed = stream->speed_target = 21;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = -1;
            stream->clickable = 1;
            stream->stream_images = 2;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        stream = streams[2];
        if(stream) {
            stream->speed = stream->speed_target = 31;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = 1;
            stream->clickable = 1;
            stream->stream_images = 3;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        stream = streams[3];
        if(stream) {
            stream->speed = stream->speed_target = 31;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = -1;
            stream->clickable = 1;
            stream->stream_images = 5;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        stream = streams[4];
        if(stream) {
            stream->speed = stream->speed_target = 34;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = 1;
            stream->clickable = 1;
            stream->stream_images = 9;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        stream = streams[5];
        if(stream) {
            stream->speed = stream->speed_target = 35;
            stream->fader = stream->fader_target = 1.0;
            stream->speed_dir = -1;
            stream->clickable = 1;
            stream->stream_images = 10;
            stream->zoom = 0;
            stream->fbopos = 0;
        }
        
        modeAdvance(0,0,0);
        
    }
    
    
    void streamSlow() {
        if(!cube || !stream) return;
        
        // 1) Move to end of display so it appears ON TOP of other art
        for(vector<Region*>::iterator it = regions.begin(); it<regions.end(); it++) {
            if(*it == stream) {
                regions.erase(it);
            }
        }
        regions.push_back(stream);
        
        // make sure mouth is last
        for(vector<Region*>::iterator it = regions.begin(); it<regions.end(); it++) {
            if(*it == mouth) {
                regions.erase(it);
            }
        }
        regions.push_back(mouth);
        
        // 2) Slow down this stream
        stream->speed_target = 3;
        
        // 3) Begin Zoom in ( TODO improve )
        stream->zoom = 1;
        
        // 4) we are expecting an up event now
        expectingdown = 0;
    }
    
    void streamPicked() {
        if(!cube || !stream) return;
        
        // we are done with this face - do not allow it to be random
        cube->facelock[picked%6] = 1;
        
        // 10) Speed up stream
        stream->speed_target = (rand()%10) + 20;
        
        // 11) Zoom out ( TODO smooth out )
        stream->zoom = 0;
        
        // 12) Remap to real texture ( TODO do in pieces at the right time )
        try {
            char blah[512];
            sprintf(blah,"/zerotheoremshared/streams/big/%d/%d.png",stream->stream_images,stream->stream_tile);
            stream->stream_cubeface = (picked%6);
            int focus = (picked%6)*3;
            cube->textures[focus+0] = cube->textures[focus+1] = cube->textures[focus+2] = Surface( loadImage(blah)); // TODO MORE ARTFULLY
            //console() << "cube face got art " << blah << endl;
        } catch( const std::exception &e  ) {
            console() << "!!! error loading stream art!!! " << e.what() << endl;
            console() << "!!! Bad remap raw image data ! : " << std::endl;
        }
        
        // 13) TODO kick off nice animations for conveyer belt
        
        // 14) Overall advance the cube to the next picking surface... for keyboard only
        picked = (picked+1)%6;
        
        // 15) we are expecting a down event now
        expectingdown = 1;
        
        // 16) we are not on a stream now
        stream = 0;
        
    }
    
    
    void modeTimer() {
        if(!cube) return;
        
        // cube face randomization periodically if unlocked - using data from an arbitrary stream
        for(int i = 0; i < 6;i++) {
            if(cube->facelock[i] || !streams[i]) continue;
            // swap the face if it is hidden
            if(((cube->texindex+i) & 7) == 7) { // TODO is this the right time?
                char blah[512];
                try {
                    int faceroll = rand()%33; //(stream->faceroll+1)%33;
                    if(i == 1 || i == 2 || i == 4 || i == 5) {
                        // these ones are actually cut vertically!
                        sprintf(blah,"/zerotheoremshared/streams/vert/%d/%d.png",streams[i]->stream_images,faceroll); // TODO JPEG
                    } else {
                        // these ones are cut horizontally... sigh.
                        sprintf(blah,"/zerotheoremshared/streams/horiz/%d/%d.png",streams[i]->stream_images,faceroll); // TODO JPEG
                    }
                    int focus = i*3 + (cube->texindex % 3); // TODO right face?
                    cube->textures[focus] = Surface( loadImage(blah));
                    //console() << "face at got art " << blah;
                } catch( const std::exception &e  ) {
                    console() << "!!! error loading art!!! " << blah << " " << e.what() << endl;
                }
            }
        }
        
        if(cube->texindex == CUBE_ERROR ) {
            // TODO should correct this more precisely at right time
            // allow this face to go random
            cube->facelock[0] = 0;
            /*
             try {
             char blah[512];
             sprintf(blah,"/zerotheoremshared/cube2/TRIANG_logos/mancom_0%d.jpg",1);
             //sprintf(blah,"/zerotheoremshared/streams/%d/%d.png",stream->stream_id+1,stream->stream_tile);
             int focus = 0;
             cube->textures[focus+0] = cube->textures[focus+1] = cube->textures[focus+2] = Surface( loadImage(blah));
             //console() << "face at got art " << blah;
             } catch( const std::exception &e  ) {
             console() << "!!! error loading mancom default art!!! " << e.what() << endl;
             console() << "!!! Bad remap raw image data ! : " << std::endl;
             }
             */
        }
        
        // cube implicitly does this now
        //if(cube->texindex == CUBE_WHITE) {
        //    cube->fader_target = 0;
        //}
        
        if(cube->texindex == CUBE_END) {
            modeReset();
        }
    }
    
    void modeAdvance(int mousex,int mousey, int mouseDownNow) {
        if(!cube) return;
        
        if(mousex || mousey) {
            
            if(!mouseDownNow) {
                if(!mousedown) return;
                mousedown = 0;
                console() << " up advanced stream to " << mode << endl;
            }
            
            else {
                if(mousedown) return; // don't allow double downs
                mousedown = 1;
                if(!expectingdown) return; // ignore you if you are not down
                
                if(!stream) {
                    
                    // find closest chosen region
                    int distance = WIDTH*WIDTH+HEIGHT*HEIGHT;
                    Region* closest = 0;
                    for(int i = 0; i< 6; i++) {
                        Region* region = streams[i];
                        if(!region || !region->clickable || !region->kind == REGION_STREAM)continue;
                        if( mousex >= region->x && mousex <= region->x + region->w && mousey >= region->y && mousey <= region->y + region->h) {
                            int d = (region->x+region->w/2-mousex)*(region->x+region->w/2-mousex);
                            if(distance > d) {
                                closest = region;
                                distance = d;
                            }
                        }
                    }
                    
                    if(closest && closest->kind == REGION_STREAM) {
                        stream = closest;
                        console() << " down advanced stream to " << mode << endl;
                    }
                }
            }
            
        } else {
            stream = streams[picked%6];
            mouseDownNow = expectingdown;
        }
        
        //console() << " picked stream: " << picked << " mouse: " << expectingdown << " cubetime: " << cube->texindex << " low: " << cube->texlow << " mode: " << mode << endl;
        
        switch(mode) {
                
            case 0:
                // we start here with a mancube blank faces thing
                cube->texlow = CUBE_START;
                cube->texhigh = 96;
                mode = 5;
                expectingdown = 1;
                //magic now cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 5:
                // somehow we get here - wait for first face to be solved
                if(!mouseDownNow) break;
                cube->texlow = 97;
                cube->texhigh = 120;
                mode = 10;
                expectingdown = 0;
                cube->facelock[0]=cube->facelock[1]=cube->facelock[2]=cube->facelock[3]=cube->facelock[4]=cube->facelock[5]=0;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
            case 10:
                if(cube->texindex<cube->texlow) break;
                expectingdown = 1;
                mode = 20;
                stream = 0;
                break;
                
            case 20:
                // qohen has selected to browse a stream so we zoom the chosen stream ( hardcoded for now ) #1 of 7
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 30;
                break;
            case 30:
                streamPicked();
                cube->texlow = 296;
                cube->texhigh = 319;
                mode = 40;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 40:
                // qohen selected another stream #2 of 7
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 50;
                break;
            case 50:
                streamPicked();
                cube->texlow = 472;
                cube->texhigh = 496;
                mode = 60;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 60:
                // qohen selected another stream #3 of 7
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 70;
                break;
            case 70:
                streamPicked();
                cube->texlow = 663;
                cube->texhigh = 686;
                mode = 80;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 80:
                // qohen selected another stream #4 of 7
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 90;
                break;
            case 90:
                streamPicked();
                cube->texlow = 792;
                cube->texhigh = 815;
                mode = 100;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 100:
                // qohen selected another stream #5 of 7
                if(cube->texindex<cube->texlow) break;
                streamSlow();
                mode = 110;
                break;
            case 110:
                streamPicked();
                /*
                 cube->texlow = 949;
                 cube->texhigh = 972;
                 mode = 120;
                 break;
                 
                 case 120:
                 // there is a pause here for a fail effect - TODO do we want a pause?
                 mode = 130;
                 break;
                 case 130:
                 */
                cube->texlow = 1106;
                cube->texhigh = 1129;
                mode = 140;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 140:
                // qohen selected another stream #6 of 7
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 150;
                break;
            case 150:
                streamPicked();
                cube->texlow = 1281;
                cube->texhigh = 1304;
                mode = 160;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 160:
                // qohen selected another stream #7 of 7 ( picking stream zero )
                if(cube->texindex<cube->texlow) break;
                if(!mouseDownNow) break;
                streamSlow();
                mode = 170;
                break;
            case 170:
                streamPicked();
                cube->texlow = 1504;  // TODO this loop is wrong??? we really should have a fade to white cube here.
                cube->texhigh = 1600;
                mode = 180;
                //cube->startThreadLoad(CUBE_REMAP_FILES,cube->texlow,cube->texhigh);
                break;
                
            case 180:
                break;
                
            default:
                break;
        }
        
        // console() << "... picked stream: " << picked << " mouse: " << expectingdown << " cubetime: " << cube->texindex << " low: " << cube->texlow << " mode: " << mode << endl;
        
    }
    
    /*
     #define NMODES 10
     int looping[NMODES*2] = {
     0, 1600,  // unused
     1,   96, // a long mancom loop
     97,  120, // mancom pause before solving 1
     296,  319, // pause before 2
     472,  496, // pause before 3
     663,  686, // pause before 4
     792,  815, // pause before 5
     949,  972, // pause before fail
     1106,1129, // pause before solving 6
     1281,1304, // pause before solving side 1 retry due to fail
     };
     */
    
    
    void update() {
        modeTimer();
    }
    
    void keyDown( KeyEvent event ) {
        switch(event.getChar()) {
            case 'r': modeReset(); break;
            case 'm':
                if(mouth) {
                    mouth->visible = 1-mouth->visible;
                    mouth->visible ? mouth->play() : mouth->stop();
                    break;
                }
            default: break;
        }
    }
    
    void mouseDown (MouseEvent event) {
        //modeAdvance(0,0,0); // for production
        modeAdvance(event.getX(),event.getY(),1);
    }
    
    void mouseUp (MouseEvent event) {
        modeAdvance(event.getX(),event.getY(),0);
    }
    
};


Group* NewCubeGroup() {
    return new CubeGroup();
}
