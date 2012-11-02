
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"

#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Capture.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"

#include "cinder/Thread.h"
#include "cinder/ConcurrentCircularBuffer.h"

#include <vector>
#include "DeckLinkAPI.h"
#include "DeckLinkController.h"


//#include "CinderOpenCv.h"

#include "sys/mman.h"

//#include "cinder/Camera.h"
using namespace ci;
using namespace ci::app;
using namespace std;

#define WIDTH 1920
#define HEIGHT 1080
#define DEFAULTFOLDER "/zerotheoremshared"
#define FRAMERATE_IDEAL 25.0f
#define FRAMERATE_MEDIAN 1.0f

class Group {
public:
	virtual void setup() {}
    virtual void update() {}
    virtual void draw() {}
    virtual void keyDown( KeyEvent event ) {    }
    virtual void mouseDown (MouseEvent event) {    }
    virtual void mouseUp (MouseEvent event) { }
    
};

extern float framerate;



