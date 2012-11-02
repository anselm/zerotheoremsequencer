
#include "ZeroTheorem.h"
#include "Region.h"
#include "Deck.h"
#include "Cube.h"

#include "cinder/CinderResources.h"
#define RES_VERTS CINDER_RESOURCE( ../, verts.glsl, 128, GLSL )
#define RES_CUBE  CINDER_RESOURCE( ../, cube.glsl, 129, GLSL )
#define RES_CUBE2 CINDER_RESOURCE( ../, cube2.glsl, 129, GLSL )

vector<class Region*> regions;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Regions; these specify what we render
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


Region::Region(int _x,int _y,int _w,int _h,int _ntextures,int _kind, string _filename) {
    
    this->x = _x;
    this->y = _y;
    this->w = _w;
    this->h = _h;
    this->kind = _kind;
    this->ntextures = _ntextures;
    this->filename = _filename;
    this->texindex = 0;
    this->texlow = this->texhigh = 0;
    this->tw = this->th = 0;
    
    switch(kind) {
            
        case REGION_MOVIE:
            try {
                movie = qtime::MovieGl(filename);
                w = movie.getWidth(); // throw away the source
                h = movie.getHeight();
                nframes = movie.getNumFrames();
                reset();
                /// TEST! try prime the pump - can we remove?
                movie.play();
                movie.stop();
                console() << " ============ loaded movie " << filename << " =================== "<< std::endl;
                console() << "Dimensions:" << movie.getWidth() << " & " << movie.getHeight() << std::endl;
                console() << "Duration:  " << movie.getDuration() << " seconds" << std::endl;
                console() << "Frames:    " << movie.getNumFrames() << std::endl;
                console() << "Framerate: " << movie.getFramerate() << std::endl;
            } catch( ... ) {
                console() << "Unable to load the movie: " << filename << std::endl;
            }
            break;
            
        case REGION_REMAP:
        {
            /*
             // do we have enough simultaneous texture units?
             int MaxTextureUnits;
             glGetIntegerv(GL_MAX_TEXTURE_UNITS, &MaxTextureUnits);
             int MaxTextureImageUnits;
             glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
             int MaxCombinedTextureImageUnits;
             glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
             if( MaxTextureUnits < 32 || MaxTextureImageUnits < 32 || MaxCombinedTextureImageUnits < 32) {
             console() << " we hit a hard limit " << MaxTextureUnits << " " << MaxTextureImageUnits << endl;
             console() << " we hit a hard limit " << MaxCombinedTextureImageUnits << endl;
             exit(0);
             }
             */
            
            // our remap shader
            try {
                shader = gl::GlslProg(loadResource(RES_VERTS),loadResource(RES_CUBE));
                shader2 = gl::GlslProg(loadResource(RES_VERTS),loadResource(RES_CUBE2));
            } catch( const std::exception &e  ) {
                console() << "error loading shader " << e.what() << endl;
            }
            
            //console() << "done loading shader " << shader << std::endl;
            break;
        }
        case REGION_CUBES:
            break;
        case REGION_IMAGE:
            try {
                Surface mytest = Surface(loadImage(filename));
                tex0 = mytest;
                w = tex0.getWidth();
                h = tex0.getHeight();
                console() << "Loaded image " << filename << " " << tex0.getWidth() << std::endl;
            } catch(...) {
                console() << "Bad image data for image " << filename << std::endl;
                exit(0);
            }
            break;
        case REGION_VIDEO:
            if(1) {
                vector<Capture::DeviceRef> devices( Capture::getDevices() );
                for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
                    Capture::DeviceRef device = *deviceIt;
                    console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
                }
                for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
                    Capture::DeviceRef device = *deviceIt;
                    console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
                    try {
                        if( device->checkAvailable() ) {
                            mCapture = Capture( w, h, device ) ;
                            mCapture.start();
                            break;
                        }
                        else {
                            console() << "device is NOT available" << std::endl;
                        }
                    }
                    catch( CaptureExc & ) {
                        console() << "Unable to initialize device: " << device->getName() << endl;
                    }
                }
            }
            break;
            
        case REGION_DECKV:
            if(1) {
                deckvideo = new DeckLinkController();
                deckvideo->init();
                if( deckvideo->getDeviceCount()) {
                    deckvideo->getDeviceNameList();
                    deckvideo->selectDevice(DECKDEVICE);
                    deckvideo->getDisplayModeNames();
                    if(deckvideo->startCapture(DECKVIDEOMODE)) break;
                }
                if(deckvideo){
                    delete(deckvideo);
                    deckvideo = 0;
                }
                console() << " deck video status is " << (deckvideo ? "on" : "off" ) << endl;
            }
            break;
            
        case REGION_WORDS:
            break;
        case REGION_STREAM:
            
            // a shader
            if(1) {
                char* verts = (char*)"void main() { gl_TexCoord[0] = gl_MultiTexCoord0; gl_Position = ftransform(); }";
                char* frags = (char*)
                "uniform vec4 mouse;\n"
                "uniform sampler2D tex0;\n"
                "uniform float fader;\n"
                "void main(void) {\n"
                "   float R = 0.4;\n"
                "   float h = 0.3;\n"
                "   float hr = R * sqrt(1.0 - ((R - h) / R) * ((R - h) / R));\n"
                "   vec2 xy = gl_TexCoord[0].xy - mouse.xy;\n"
                "   float r = sqrt(xy.x * xy.x + xy.y * xy.y);\n"
                "   vec2 new_xy = r < hr ? xy * (R - h) / sqrt(R * R - r * r) : xy;\n"
                "   gl_FragColor = texture2D(tex0, new_xy + mouse.xy ) * fader;\n"
                "}\n";
                shader = gl::GlslProg( verts, frags );
            }
            
            break;
        case REGION_MASK:
            try {
                Surface temp = Surface(loadImage(filename));
                Surface temp2 = Surface(w,h,false);
                temp2.copyFrom(temp,Area(x,y,x+w,y+h),Vec2i(-x,-y));
                tex0 = temp2;
            } catch(...) {
                console() << "Bad mask image data" << std::endl;
                exit(0);
            }
            break;
        case REGION_FPS:
            break;
        default:
            break;
            
    }
    
    /* mesh builder unused
     float n = 8;
     for(float i = 0; i < n; i++) {
     float x1,x2,y1,y2;
     x1 = tw/n*i;
     x2 = tw/n*(i+1);
     y1 = 0;
     y2 = th;
     mesh.appendVertex  ( Vec3f(x1,y1,0) );
     mesh.appendVertex  ( Vec3f(x1,y2,0) );
     mesh.appendVertex  ( Vec3f(x2,y2,0) );
     mesh.appendVertex  ( Vec3f(x2,y1,0) );
     mesh.appendColorRGB( Color(1,1,1));
     mesh.appendColorRGB( Color(1,1,1));
     mesh.appendColorRGB( Color(1,1,1));
     mesh.appendColorRGB( Color(1,1,1));
     x1 = 1.0f/n*i;
     x2 = 1.0f/n*(i+1);
     y1 = 0;
     y2 = 1.0f;
     mesh.appendTexCoord( Vec2f(x1,y1));
     mesh.appendTexCoord( Vec2f(x1,y2));
     mesh.appendTexCoord( Vec2f(x2,y2));
     mesh.appendTexCoord( Vec2f(x2,y1));
     mesh.appendTriangle(0+4*i,1+4*i,2+4*i);
     mesh.appendTriangle(0+4*i,2+4*i,3+4*i);
     }
     */
}

void Region::load_queue() {
    sequencebitlen = 0;
    for(int j = 0; j < ntextures;j++) {
        char blah[512];
        sprintf(blah,filename.c_str(),stream_images,j);
        //console() << "Loading " << j << " " << filename << endl;
        try {
            Surface32f surface = loadImage( blah );
            if(surface.getWidth()<=32 || surface.getHeight()<=32 || surface.getWidth()>4096 || surface.getHeight()>4096) {
                console() << "Image " << j << " " << blah << "error - has implausible width or height" << std::endl;
                continue;
            }
            if(!tw && !th) {
                tw = surface.getWidth();
                th = surface.getHeight();
            } else if(tw != surface.getWidth() || th != surface.getHeight()) {
                console() << "Image " << j << " " << blah << "error - has not the same width or height as other frames" << std::endl;
                continue;
            }
            //console() << "Done loading " << j << " " << blah<< std::endl;
            textures.push_back(gl::Texture(surface));
            textureoffsets.push_back(sequencebitlen);
            sequencebitlen += surface.getWidth();
        } catch( ... ) {
            console() << "Image " << j << " " << blah << "error - failed to load!!!" << std::endl;
        }
    }
}

void Region::update(float ratio) {
    switch(kind) {
        case REGION_WORDS:
            if(!loaded) {
                loaded = 1;
                terms = split(filename,' ');
                sequencebitlen = 0;
                for(int i = 0; i < terms.size(); i++) {
                    std::string normalFont( "Courier New Bold" );
                    TextLayout layout;
                    //    layout.clear(ColorA(0,0,0,0));
                    layout.setFont(Font( normalFont,72));
                    layout.setColor(Color(1.0,0.8,0));
                    layout.addLine(terms[i]);
                    Surface8u rendered = layout.render( true, 0 );
                    textures.push_back(gl::Texture( rendered ));
                    textureoffsets.push_back(sequencebitlen);
                    sequencebitlen += rendered.getWidth() + 30;
                }
            }
            if(1) {
                // roll down, increasing values go lower on the screen; the data itself is offscreen to the top
                scroll+=speed * speed_dir;
                // if all pieces have exactly rolled off the bottom of the screen then reset it such that the first page is on display
                if(scroll > sequencebitlen + h ) scroll = h + speed * speed_dir;
                // if the entire sequence is off the top then adjust such that the last page is on display
                if(scroll < 0) scroll = sequencebitlen + speed * speed_dir;
            }
            break;
            
        case REGION_VIDEO:
            if( mCapture && mCapture.checkNewFrame() ) {
                mSurface = mCapture.getSurface();
                mTexture = gl::Texture( mSurface );
                mValid = 1;
            }
            break;
        case REGION_DECKV:
            break;
            
        case REGION_MOVIE:
            if(playing) {
                step(rate * ratio);
            } else if(dirty) {
                step(0); // if dirty then force to a frame
            }
            break;

        case REGION_STREAM:
            
            if(1) {
                // roll down the blocks to view
                if(fbopos < h) fbopos += 10; if(fbopos > h ) fbopos = h;
                
                // advance fader targets
                if(!shader || !zoom) {
                    fader_target = 0.5;
                    fader += (fader_target-fader)/4.0;
                } else {
                    fader_target = 1.0;
                    fader += (fader_target-fader)/4.0;
                }
                
                // 0,0 is the top left corner in this case
                // roll direction, increasing + values go lower on the screen; the data itself is offscreen to the negative top direction
                speed+=(speed_target-speed)/8.0f;
                scroll+=speed * speed_dir;
                // if all pieces have exactly rolled off the bottom of the screen then reset it such that the first page is on display
                if(scroll > sequencebitlen + h ) scroll = h + speed * speed_dir;
                // if the entire sequence is off the top then adjust such that the last page is on display
                if(scroll < 0) scroll = sequencebitlen + speed * speed_dir;
            }
            break;

            
        case REGION_REMAP:

            if(1) {
                // fader targets
                fader_target = texindex < CUBE_WHITE ? 1 : 0;
                fader += (fader_target-fader)/8.0;

                // paint our sequence by streaming off disk
                if(ntextures) {
                    
                    // stay within bounds ( index 1 )
                    texindex++;
                    if(texindex>ntextures) texindex = ntextures;
                    //if(texindex<texlow) texindex = texlow;
                    if(texindex>texhigh) texindex = texlow;
                    if(texindex<CUBE_START) texindex = CUBE_START;
                    
                    // uses a cache management system now to fetch these
                    //              tex1 = cache1Load(texindex);
                    //             tex0 = cache0Load(CUBE_REMAP_FILES,texindex);
                    //tex2 = cache2Load(texindex);
                    
                }
            }
            break;

        default:
            break;
    }
}

void Region::draw() {
    
    switch(kind) {
            
        case REGION_MOVIE:
            if(visible && movie) {
                if(dirty) {
                    tex0 = movie.getTexture();
                    dirty = 0;
                }
                if(tex0) {
                    gl::enableAlphaBlending();
                    gl::color( Color::white() );
                    gl::pushMatrices();
                    gl::translate(x,y);
                    gl::rotate(rotate);
                    gl::draw( tex0, Rectf(0,0,w,h) );
                    gl::popMatrices();
                }
            }
            break;
            
        case REGION_REMAP:

            gl::enableAlphaBlending();
            
            if(ntextures && tex0 && tex1 && textures.size()) {
                int splitat = 9;
                // first shader of two
                try {
                    tex0.bind(0);
                    tex1.bind(1);
                    for(int i = 0; i < splitat; i++ ) {
                        textures[i].bind(i+2);
                    }
                    shader.bind();
                    shader.uniform("remap",0);
                    shader.uniform("pretty",1);
                    shader.uniform("fader",fader);
                    for(int i = 0; i < splitat; i++ ) {
                        char blah[256];
                        sprintf(blah,"img%d",i);
                        shader.uniform(blah,i+2);
                    }
                    gl::pushMatrices();
                    gl::translate(x,y+h);
                    gl::rotate(rotate);
                    gl::drawSolidRect(Rectf(0,0,w,h));
                    gl::popMatrices();
                    for(int i = 0; i < splitat; i++ ) {
                        textures[i].unbind();
                    }
                    tex1.unbind();
                    tex0.unbind();
                    shader.unbind();
                } catch(...) {
                }
                try {
                    tex0.bind(0);
                    tex1.bind(1);
                    for(int i = 0; i < splitat; i++ ) {
                        textures[i+splitat].bind(i+2);
                    }
                    shader2.bind();
                    shader2.uniform("remap",0);
                    shader2.uniform("pretty",1);
                    shader.uniform("fader",fader);
                    for(int i = 0; i < splitat; i++ ) {
                        char blah[256];
                        sprintf(blah,"img%d",i+splitat);
                        shader2.uniform(blah,i+2);
                    }
                    gl::pushMatrices();
                    gl::translate(x,y+h);
                    gl::rotate(rotate);
                    gl::drawSolidRect(Rectf(0,0,w,h));
                    gl::popMatrices();
                    for(int i = 0; i < splitat; i++ ) {
                        textures[i+splitat].unbind();
                    }
                    tex1.unbind();
                    tex0.unbind();
                    shader2.unbind();
                } catch(...) {
                }
            }
            break;
        case REGION_MASK:
        case REGION_IMAGE:
            if(visible) {
                gl::disableAlphaBlending();
                if(!shader) {
                    gl::draw(tex0,Rectf(x,y,x+w,y+h));
                } else {
                    gl::draw(tex0,Rectf(x,y,x+w,y+h));
                    tex0.bind(0);
                    shader.bind();
                    shader.uniform("tex0",0);
                    Vec4f mouse(WIDTH/2,HEIGHT/2,0,0);
                    shader.uniform("mouse",mouse);
                    gl::drawSolidRect(Rectf(x,y,x+w,y+h));
                    tex0.unbind();
                    shader.unbind();
                }
            }
            break;
            
        case REGION_VIDEO:
            if( mCapture && mTexture && mValid) {
                gl::draw( mTexture,Rectf(x,y,x+w,y+h));
            } else {
                console() << " no video " << endl;
            }
            break;
            
        case REGION_DECKV:
            if(!visible)return;
            if(deckvideo) {
                Surface* decksurface = deckGetRead();
                if(decksurface) {
                    tex0 = *decksurface;
                    deckReleaseRead(); // strategy here - if we hold onto the texture longer we will block more of the reader thread which is voracious...
                }
            }
            if(tex0) {
                gl::draw( tex0, Rectf(x,y,x+w,y+h));
            }
            
            if(1) {
                static int test = 0;
                if(!test && deckvideo && deckvideo->screenPreviewHelper) {
                    deckvideo->screenPreviewHelper->InitializeGL();
                    //helper->Set3DPreviewFormat(bmd3DPreviewFormatDefault);
                    test = 1;
                }
                /*
                 glViewport(0, 0, WIDTH, HEIGHT);
                 glMatrixMode(GL_PROJECTION);
                 glLoadIdentity();
                 glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
                 glMatrixMode(GL_MODELVIEW);
                 glClearColor(1,0,1,1);
                 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                 deckvideo->screenPreviewHelper->PaintGL();
                 */
                glColor4f(0,1,2,1);
                glPushMatrix();
                // looks like it is an open gl box that is 2 units wide by 2 tall
//                glTranslatef(WIDTH/2-545,HEIGHT/2-33-100,0.0f);
//                glScalef(924/2*WIDTH/HEIGHT/2,-974/2/2,-1);

                // we translate to the desired position, but we have to subtract the width and height offset
                glTranslatef(x,y,0.0f);

                // since it is a 1 by 1 box if we scale it to the w,h then we should be sized right
                glScalef(w/2.0f,h/2.0f,1.0f);

                //glRotatef(180,0,0,1);
                if(deckvideo && deckvideo->screenPreviewHelper) {
                    deckvideo->screenPreviewHelper->PaintGL();
                } else {
                    glColor4f(0.1,0.6,0.1,1);
                    glRectf(-1,-1,1,1);
                }
                
                glPopMatrix();
            }
            
            break;
            
        case REGION_WORDS:
            // 0,0 is the top left corner in this case
            gl::enableAlphaBlending();
            gl::color( Color::white() );
            for(int i = 0; i < textures.size() ; i++ ) {
                // each piece begins at the end of the previous segment such that the first segment is at zero, and grows in negative direction
                int ypos = scroll-textureoffsets[i];
                // as soon as a piece has been pushed off the bottom of the screen in positive space, wrap it back to the top to smooth loop display
                if(ypos > sequencebitlen) ypos -= sequencebitlen;
                // as soon as a piece has gone off the top tack it to the bottom
                if(ypos < 0) ypos += sequencebitlen;
                // only bother painting pieces that are actually on the display
                if(ypos > 0 && ypos-textures[i].getWidth() < h) {
                    // print each piece rotating into place
                    gl::pushMatrices();
                    // translate to absolute space where we wish to print x1,y2,x2,y2 region
                    gl::translate(x,y+ypos);
                    // rotate in place after translation such that the element y2 is < y1
                    gl::rotate(rotate);
                    // paint here
                    gl::draw(textures[i],Vec2f(0,0));
                    gl::popMatrices();
                }
            }
            break;

        case REGION_STREAM:
            if(1) {
                //  mogrify -strip -interlace Plane -gaussian-blur 0.05 -quality 85% -format jpg -repage -crop 512x512+84+84 *.tif
    
                // an fbo
                if(!fbo || fbopos != h) {
                    fbo = gl::Fbo(w,fbopos,1,1,0);
                }
                
                gl::SaveFramebufferBinding bindingSaver;
                if(fbo) {
                    fbo.bindFramebuffer();
                    gl::enableAlphaBlending();
                    gl::clear(ColorA(0,0,0,0.0f));
                    gl::color(ColorA(1,1,1,1));
                }
                
                for(int i = 0; i < textures.size() ; i++ ) {
                    // each piece begins at the end of the previous segment such that the first segment is at zero, and grows in negative direction
                    int ypos = scroll-textureoffsets[i];
                    // as soon as a piece has been pushed off the bottom of the screen in positive space, wrap it back to the top to smooth loop display
                    if(ypos > sequencebitlen) ypos -= sequencebitlen;
                    // as soon as a piece has gone off the top tack it to the bottom
                    if(ypos < 0) ypos += sequencebitlen;
                    // only bother painting pieces that are actually on the display
                    if(ypos > 0 && ypos-textures[i].getWidth() < h) {
                        // print each piece rotating into place
                        gl::pushMatrices();
                        // translate to absolute space where we wish to print x1,y2,x2,y2 region
                        if(!fbo) {
                            // for our vbo we print into the center x so we can apply a shader
                            gl::translate(x+w/3,y+ypos);
                        } else {
                            // fbos are oriented such that 0,0 is at the BOTTOM! and they are erroneously set to full screen height...
                            gl::translate(w/3,HEIGHT-ypos+th);
                        }
                        // rotate in place after translation such that the element y2 is < y1
                        gl::rotate(rotate);
                        // paint here
                        gl::draw(textures[i],Vec2f(0,0));
                        gl::popMatrices();
                        // what image is in the middle?
                        if(ypos > h/2-th/2 && ypos < h/2+th/2) stream_tile = i;
                    }
                }
                
            }
            
            if(!shader || !zoom) {
                gl::enableAlphaBlending();
                gl::color(ColorA(1,1,1,fader));
                gl::draw(fbo.getTexture(0),Rectf(x,y+(h-fbopos),x+w,y+h));
                gl::color(ColorA(1,1,1,1));
            } else {
                gl::enableAlphaBlending();
                gl::color(ColorA(1,1,1,1));
                fbo.getTexture(0).bind(0);
                shader.bind();
                shader.uniform("fader",fader);
                shader.uniform("tex0",0);
                shader.uniform("mouse",Vec4f(0.5f,0.5f,0,0));
                gl::drawSolidRect(Rectf(x,y+(h-fbopos),x+w,y+h));
                fbo.getTexture(0).unbind();
                shader.unbind();
            }
            
            break;
        case REGION_FPS:
            if(1) {
                std::string normalFont( "Courier New Bold" );
                TextLayout layout;
                layout.setFont(Font( normalFont,22));
                layout.setColor(Color(1.0,0.8,0));
                char buffer[200];
                sprintf(buffer,"%f",framerate );
                layout.addLine(buffer);
                Surface8u rendered = layout.render( true, 0 );
                gl::pushMatrices();
                gl::translate(x,y);
                gl::rotate(rotate);
                gl::draw(rendered,Rectf(0,0,w,h));
                gl::popMatrices();
            }
            break;
        default:
            break;
    }
}
