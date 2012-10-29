
#include "ZeroTheorem.h"
#include "Deck.h"

static Surface decksurface1 = Surface(DECKWIDTH/DECKRATIO,DECKHEIGHT/DECKRATIO,0,SurfaceChannelOrder::RGB);
static Surface decksurface2 = Surface(DECKWIDTH/DECKRATIO,DECKHEIGHT/DECKRATIO,0,SurfaceChannelOrder::RGB);
static Surface decksurface3 = Surface(DECKWIDTH/DECKRATIO,DECKHEIGHT/DECKRATIO,0,SurfaceChannelOrder::RGB);
static Surface decksurface4 = Surface(DECKWIDTH/DECKRATIO,DECKHEIGHT/DECKRATIO,0,SurfaceChannelOrder::RGB);

static Surface* decksurfaces[NDECKS] = { &decksurface1, &decksurface2, &decksurface3, &decksurface4 };
static int deckstatus[NDECKS] = { DECK_EMPTY, DECK_EMPTY };
static mutex deckmutex;
static int deckreader = 0;
static int deckwriter = 0;

static Timer decktimer;
static Timer decktimertotal;

Surface* deckGetRead() {
    Surface* surface = 0;
    deckmutex.lock();
    if(deckstatus[deckreader] == DECK_FULL) {
        deckstatus[deckreader] = DECK_READER;
        surface = decksurfaces[deckreader];
    }
    deckmutex.unlock();
    return surface;
}

void deckReleaseRead() {
    deckstatus[deckreader] = DECK_EMPTY;
    deckreader = ( deckreader +1 ) % NDECKS;
}

static Surface *deckGetWrite() {
    Surface* surface = 0;
    deckmutex.lock();
    if(deckstatus[deckwriter] == DECK_EMPTY) {
        deckstatus[deckwriter] = DECK_WRITER;
        surface = decksurfaces[deckwriter];
    }
    deckmutex.unlock();
    return surface;
}

static void deckReleaseWrite() {
    deckstatus[deckwriter] = DECK_FULL;
    deckwriter = ( deckwriter +1 ) % NDECKS;
}

static const int precision=32768;
static const int coefficientY=(int)(1.164*precision+0.5);
static const int coefficientRV = (int)(1.596*precision+0.5);
static const int coefficientGU = (int)(0.391*precision+0.5);
static const int coefficientGV = (int)(0.813*precision+0.5);
static const int coefficientBU = (int)(2.018*precision+0.5);

static void deckUYVYtoRGB(Surface* surface, unsigned char* pData) {
    
    // decode bmdFormat8BitYUV http://stackoverflow.com/questions/7954416/converting-yuv-into-bgr-or-rgb-in-opencv
    // this is uYvY apparently?
    // http://www.sitemcr.com/softphone/libminisip/source/subsystem_media/video/grabber/DeckLinkGrabber.cxx <- best
    
    if(surface && pData) {
        int x = 0;
        int y = 0;
        float U,Y1,V,Y2;
        int R,G,B;
        Surface8u::Iter iter = surface->getIter();
        while( iter.line() ) {
            x = 0;
            while(true) {
                
                if(!iter.pixel())break;
                int j = y*DECKSTRIDE + x*4;
                U = pData[j+0];
                Y1 = pData[j+1];
                V = pData[j+2];
                Y2 = pData[j+3];
                
                R = coefficientY*(Y1-16)+coefficientRV*(V-128);
                G = coefficientY*(Y1-16)-coefficientGU*(U-128)-coefficientGV*(V-128);
                B = coefficientY*(Y1-16)+coefficientBU*(U-128);
                R = (R+precision/2)/precision;
                G = (G+precision/2)/precision;
                B = (B+precision/2)/precision;
                if (R<0) R=0;
                if (G<0) G=0;
                if (B<0) B=0;
                if (R>255) R=255;
                if (G>255) G=255;
                if (B>255) B=255;
                iter.r() = R;
                iter.g() = G;
                iter.b() = B;
                
                if(!iter.pixel())break;
                R = coefficientY*(Y2-16)+coefficientRV*(V-128);
                G = coefficientY*(Y2-16)-coefficientGU*(U-128)-coefficientGV*(V-128);
                B = coefficientY*(Y2-16)+coefficientBU*(U-128);
                R = (R+precision/2)/precision;
                G = (G+precision/2)/precision;
                B = (B+precision/2)/precision;
                if (R<0) R=0;
                if (G<0) G=0;
                if (B<0) B=0;
                if (R>255) R=255;
                if (G>255) G=255;
                if (B>255) B=255;
                iter.r() = R;
                iter.g() = G;
                iter.b() = B;
                
                x+=DECKRATIO;
            }
            y+=DECKRATIO;
        }
    }
}

static int deckframe = 0;
static int deckframeused = 0;

void DeckLinkController::getAncillaryDataFromFrame(IDeckLinkVideoInputFrame* videoFrame, BMDTimecodeFormat timecodeFormat, NSString** timecodeString, NSString** userBitsString) {
    deckframe++;
    decktimer.start();
    if(decktimertotal.isStopped()) decktimertotal.start();
	if ((videoFrame != NULL)) {
        Surface* surface = deckGetWrite();
        if(surface) {
            int w,h,format,rowbytes,flags;
            void* buffer = 0;
            videoFrame->GetBytes(&buffer);
            w = videoFrame->GetWidth();
            h = videoFrame->GetHeight();
            if(buffer && w == DECKWIDTH && h == DECKHEIGHT) {
                format = videoFrame->GetPixelFormat();
                rowbytes = videoFrame->GetRowBytes();
                flags = videoFrame->GetFlags();
                if(rowbytes == DECKWIDTH*2) {
                    deckframeused++;
                    deckUYVYtoRGB(surface,(unsigned char*)buffer);
                    //NSLog(@"got a frame with %d %d %s %d %d",w,h,format == bmdFormat8BitYUV ? "yuv format" : "some other format",rowbytes,flags);
                }
            }
            deckReleaseWrite();
        }
	}
    
    {
        //   int handle = shm_open("mymemory",O_RDWR);
    }
    
    decktimer.stop();
    console() << " data read of frame " << deckframe << " of used " << deckframeused << " took " << decktimer.getSeconds() << " of total " << decktimertotal.getSeconds() <<  endl;
}


