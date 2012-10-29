
#include <vector>
#include "DeckLinkAPI.h"

class MyGLScreenCallback: public IDeckLinkScreenPreviewCallback {
public:
    IDeckLinkGLScreenPreviewHelper* helper;
    HRESULT DrawFrame(/* in */ IDeckLinkVideoFrame *theFrame) {
        if(helper)helper->SetFrame(theFrame);
        return 0;
    }
	virtual HRESULT		QueryInterface (REFIID iid, LPVOID *ppv)	{return E_NOINTERFACE;}
	virtual ULONG		AddRef ()									{return 1;}
	virtual ULONG		Release ()									{return 1;}
};

class DeckLinkController : public IDeckLinkInputCallback
{
private:
	std::vector<IDeckLink*>			deviceList;
	IDeckLink*						selectedDevice;
	IDeckLinkInput*					deckLinkInput;
	std::vector<IDeckLinkDisplayMode*>	modeList;
	
	bool							supportFormatDetection;
	bool							currentlyCapturing;
		
	void				getAncillaryDataFromFrame(IDeckLinkVideoInputFrame* frame, BMDTimecodeFormat format, NSString** timecodeString, NSString** userBitsString);

public:
	MyGLScreenCallback              mygl;
    IDeckLinkGLScreenPreviewHelper* screenPreviewHelper;

	DeckLinkController();
	virtual ~DeckLinkController();
	
	bool				init();
	
	int					getDeviceCount();
	
	bool				selectDevice(int index);
	
	void                getDeviceNameList();
	void                getDisplayModeNames();
	bool				isFormatDetectionEnabled();
	bool				isCapturing();
	
	bool				startCapture(int videoModeIndex);
	void				stopCapture();
    
	//
	// IDeckLinkInputCallback interface
	
	// IUnknown needs only a dummy implementation
	virtual HRESULT		QueryInterface (REFIID iid, LPVOID *ppv)	{return E_NOINTERFACE;}
	virtual ULONG		AddRef ()									{return 1;}
	virtual ULONG		Release ()									{return 1;}
	
	virtual HRESULT		VideoInputFormatChanged (/* in */ BMDVideoInputFormatChangedEvents notificationEvents, /* in */ IDeckLinkDisplayMode *newDisplayMode, /* in */ BMDDetectedVideoInputFormatFlags detectedSignalFlags);
	virtual HRESULT		VideoInputFrameArrived (/* in */ IDeckLinkVideoInputFrame* videoFrame, /* in */ IDeckLinkAudioInputPacket* audioPacket);	
};

