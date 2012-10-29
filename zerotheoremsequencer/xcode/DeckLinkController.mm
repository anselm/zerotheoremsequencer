
#import <Foundation/Foundation.h>

#include "DeckLinkController.h"

using namespace std;

DeckLinkController::DeckLinkController()
	: selectedDevice(NULL), deckLinkInput(NULL), supportFormatDetection(false), currentlyCapturing(false), screenPreviewHelper(NULL)
{}


DeckLinkController::~DeckLinkController()
{
	vector<IDeckLink*>::iterator it;
	
    // Release screen preview
	if (screenPreviewHelper != NULL) {
		screenPreviewHelper->Release();
		screenPreviewHelper = NULL;
	}
    
	// Release the IDeckLink list
	for(it = deviceList.begin(); it != deviceList.end(); it++)
	{
		(*it)->Release();
	}
}


bool DeckLinkController::init()
{
	IDeckLinkIterator*	deckLinkIterator = NULL;
	IDeckLink*			deckLink = NULL;
	bool				result = false;
	
	// Create an iterator
	deckLinkIterator = CreateDeckLinkIteratorInstance();
	if (deckLinkIterator == NULL)
	{
		goto bail;
	}
	
	// List all DeckLink devices
	while (deckLinkIterator->Next(&deckLink) == S_OK)
	{
		// Add device to the device list
		deviceList.push_back(deckLink);
	}
	
	if (deviceList.size() == 0)
	{
		goto bail;
	}

    screenPreviewHelper = CreateOpenGLScreenPreviewHelper();

	result = true;
	
bail:
	if (deckLinkIterator != NULL)
	{
		deckLinkIterator->Release();
		deckLinkIterator = NULL;
	}
	
	return result;
}


int			DeckLinkController::getDeviceCount()
{
	return deviceList.size();
}


void		DeckLinkController::getDeviceNameList()
{
	NSMutableArray*		nameList = [NSMutableArray array];
	int					deviceIndex = 0;
	
	while (deviceIndex < deviceList.size())
	{		
		CFStringRef	cfStrName;
		
		// Get the name of this device
		if (deviceList[deviceIndex]->GetDisplayName(&cfStrName) == S_OK)
		{		
			[nameList addObject:(NSString *)cfStrName];
			CFRelease(cfStrName);
            NSLog(@"found device %@",cfStrName);
		}
		else
		{
			[nameList addObject:@"DeckLink"];
		}

		deviceIndex++;
	}
	
}


bool		DeckLinkController::selectDevice(int index)
{
	IDeckLinkAttributes*			deckLinkAttributes = NULL;
	IDeckLinkDisplayModeIterator*	displayModeIterator = NULL;
	IDeckLinkDisplayMode*			displayMode = NULL;
	bool							result = false;

	// Check index
	if (index >= deviceList.size())
	{
		goto bail;
	}
	
	// A new device has been selected.
	// Release the previous selected device and mode list
	if (deckLinkInput != NULL)
		deckLinkInput->Release();
	
	while(modeList.size() > 0) {
		modeList.back()->Release();
		modeList.pop_back();
	}
	
	// Get the IDeckLinkInput for the selected device
	if ((deviceList[index]->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK))
	{
		deckLinkInput = NULL;
		goto bail;
	}
	
	//
	// Retrieve and cache mode list	
	if (deckLinkInput->GetDisplayModeIterator(&displayModeIterator) == S_OK)
	{
		while (displayModeIterator->Next(&displayMode) == S_OK)
			modeList.push_back(displayMode);

		displayModeIterator->Release();
	}
	
	//
	// Check if input mode detection format is supported.
	
	supportFormatDetection = false;	// assume unsupported until told otherwise
	if (deviceList[index]->QueryInterface(IID_IDeckLinkAttributes, (void**) &deckLinkAttributes) == S_OK)
	{	
		if (deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &supportFormatDetection) != S_OK)
			supportFormatDetection = false;
		
		deckLinkAttributes->Release();
	}
	
	result = true;
	
bail:
	return result;
}

void	DeckLinkController::getDisplayModeNames()
{
	NSMutableArray*		modeNames = [NSMutableArray array];
	int					modeIndex;
	CFStringRef			modeName;
	
	for (modeIndex = 0; modeIndex < modeList.size(); modeIndex++)
	{			
		if (modeList[modeIndex]->GetName(&modeName) == S_OK)
		{
			[modeNames addObject:(NSString *)modeName];
			CFRelease(modeName);
            NSLog(@"adding a mode %@",modeName);
		}
		else 
		{
			[modeNames addObject:@"Unknown mode"];
		}
	}
	
}

bool		DeckLinkController::isFormatDetectionEnabled()
{
	return supportFormatDetection;
}

bool		DeckLinkController::isCapturing()
{
	return currentlyCapturing;
}
        
bool		DeckLinkController::startCapture(int videoModeIndex)
{
	BMDVideoInputFlags		videoInputFlags;
    
    supportFormatDetection = 0;
	
	// Enable input video mode detection if the device supports it
	videoInputFlags = supportFormatDetection ? bmdVideoInputEnableFormatDetection : bmdVideoInputFlagDefault;
	
	// Get the IDeckLinkDisplayMode from the given index
	if ((videoModeIndex < 0) || (videoModeIndex >= modeList.size())) {
        NSLog(@"**** Blackmagic connect video failed to get right mode");
		return false;
	}

	// Set the screen preview
    mygl.helper = screenPreviewHelper;
	deckLinkInput->SetScreenPreviewCallback(&mygl);
    
	// Set capture callback
	//deckLinkInput->SetCallback(this);
	
    int themode = modeList[videoModeIndex]->GetDisplayMode();
    
	// Set the video input mode
	int err = deckLinkInput->EnableVideoInput(themode, bmdFormat8BitYUV, videoInputFlags);
    
    if(err != S_OK) {
        NSLog(@"**** Blackmagic connect video failed to startup with error %d",err);
		return false;
	}
	
	// Start the capture
    err = deckLinkInput->StartStreams();
    if( err != S_OK) {
        NSLog(@"**** Blackmagic stream start video failed to startup with error %d",err);
		return false;
	}
	
	currentlyCapturing = true;
	
    NSLog(@"starting to capture");
	return true;
}

void		DeckLinkController::stopCapture()
{
	// Stop the capture
	deckLinkInput->StopStreams();
	
	// Delete capture callback
	deckLinkInput->SetCallback(NULL);
	
	currentlyCapturing = false;
}


HRESULT		DeckLinkController::VideoInputFormatChanged (/* in */ BMDVideoInputFormatChangedEvents notificationEvents, /* in */ IDeckLinkDisplayMode *newMode, /* in */ BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{	
	UInt32				modeIndex = 0;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	// Find the index of the new mode in the mode list so we can update the UI
	while (modeIndex < modeList.size()) {
		if (modeList[modeIndex]->GetDisplayMode() == newMode->GetDisplayMode())
		{
			break;
		}
		modeIndex++;
	}
	

bail:
	[pool release];
	return S_OK;
}

typedef struct {
	// VITC timecodes and user bits for field 1 & 2
	NSString*	vitcF1Timecode;
	NSString*	vitcF1UserBits;
	NSString*	vitcF2Timecode;
	NSString*	vitcF2UserBits;
	
	// RP188 timecodes and user bits (VITC1, VITC2 and LTC)
	NSString*	rp188vitc1Timecode;
	NSString*	rp188vitc1UserBits;
	NSString*	rp188vitc2Timecode;
	NSString*	rp188vitc2UserBits;
	NSString*	rp188ltcTimecode;
	NSString*	rp188ltcUserBits;
} AncillaryDataStruct;

HRESULT 	DeckLinkController::VideoInputFrameArrived (/* in */ IDeckLinkVideoInputFrame* videoFrame, /* in */ IDeckLinkAudioInputPacket* audioPacket)
{
	//BOOL					hasValidInputSource = (videoFrame->GetFlags() & bmdFrameHasNoInputSource) != 0 ? NO : YES;
	AncillaryDataStruct		ancillaryData;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	// Update input source label
	
	// Get the various timecodes and userbits for this frame
	getAncillaryDataFromFrame(videoFrame, bmdTimecodeVITC, &ancillaryData.vitcF1Timecode, &ancillaryData.vitcF1UserBits);
	getAncillaryDataFromFrame(videoFrame, bmdTimecodeVITCField2, &ancillaryData.vitcF2Timecode, &ancillaryData.vitcF2UserBits);
	getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188VITC1, &ancillaryData.rp188vitc1Timecode, &ancillaryData.rp188vitc1UserBits);
	getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188LTC, &ancillaryData.rp188ltcTimecode, &ancillaryData.rp188ltcUserBits);
	getAncillaryDataFromFrame(videoFrame, bmdTimecodeRP188VITC2, &ancillaryData.rp188vitc2Timecode, &ancillaryData.rp188vitc2UserBits);
	
	
	[pool release];
	return S_OK;
}




