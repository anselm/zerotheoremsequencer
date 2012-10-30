
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
        loadstate();
    }

    void setup() {

    
        // currentGroup = new GroupCube();
        //currentGroup = new GroupBlackHole();
        currentGroup = new Script();
        currentGroup->setup();
    }

    int m = 1;
    float x=0,y=0,z=0;
    
    void savestate() {
        FILE* fp = fopen( DEFAULTFOLDER "/zerostate.txt","w");
        if(fp) {
            fprintf(fp,"%f %f %f",x,y,z);
            fclose(fp);
        }
    }
    
    void loadstate() {
        FILE* fp = fopen( DEFAULTFOLDER "/zerostate.txt","r");
        if(fp) {
            fscanf(fp,"%f %f %f",&x,&y,&z);
            fclose(fp);
        }
    }

	void keyDown( KeyEvent event ) {

        switch(event.getChar()) {
            case 'q': setFullScreen(0); showCursor(); quit(); return;
            case 'f': setFullScreen( ! isFullScreen() ); return;
            case 'm': m = 1 - m; if(!m) hideCursor(); else showCursor(); return;
            default: break;
        }

        switch(event.getCode()) {
            case KeyEvent::KEY_RIGHT:  x++; savestate(); return; //glTranslatef(1.0,0.0,0.0); return;
            case KeyEvent::KEY_LEFT:   x--; savestate(); return; //glTranslatef(-1.0,0.0,0.0); return;
            case KeyEvent::KEY_UP:    y--; savestate(); return; //glTranslatef(0.0,-1.0,0.0); return;
            case KeyEvent::KEY_DOWN:  y++; savestate(); return; //glTranslatef(0.0,1.0,0.0); return;
            case 269: z -=1.1f; savestate(); return; //glTranslatef(0.0,0.0,-1.1); return;
            case 61: z+=1.1f; savestate(); return; //glTranslatef(0.0,0.0,1.1); return;
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
        gl::pushMatrices();
        glTranslatef(x,y,z);
        for( size_t m = 0; m < regions.size(); ++m ) {
            regions[m]->draw();
        }
        framerate = this->getAverageFps();
        gl::popMatrices();
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

