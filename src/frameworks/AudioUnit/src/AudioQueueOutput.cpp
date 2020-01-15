#include <AudioUnit/AudioQueueOutput.h>
#include <CoreServices/MacErrors.h>
#include <cstdlib>
#include <cstdio>

AudioQueueOutput::AudioQueueOutput(const AudioStreamBasicDescription *inFormat,
		AudioQueueOutputCallback inCallbackProc,
	void *inUserData, CFRunLoopRef inCallbackRunLoop,
	CFStringRef inCallbackRunLoopMode, UInt32 inFlags)
: AudioQueue(inFormat, inUserData, inCallbackRunLoop, inCallbackRunLoopMode, inFlags),
		m_callback(inCallbackProc)
{
	
}

AudioQueueOutput::~AudioQueueOutput()
{
	
}

static int verbose = 0;

__attribute__((constructor))
static void initme(void) {
    verbose = getenv("STUB_VERBOSE") != NULL;
}

OSStatus AudioQueueOutput::start(const AudioTimeStamp *inStartTime)
{
	if (verbose) puts("STUB: AudioQueueOutput::start");
	return unimpErr;
}

OSStatus AudioQueueOutput::prime(UInt32 inNumberOfFramesToPrepare, UInt32 *outNumberOfFramesPrepared)
{
	if (verbose) puts("STUB: AudioQueueOutput::prime");
	return unimpErr;
}

OSStatus AudioQueueOutput::flush()
{
	if (verbose) puts("STUB: AudioQueueOutput::flush");
	return unimpErr;
}

OSStatus AudioQueueOutput::stop(Boolean inImmediate)
{
	if (verbose) puts("STUB: AAudioQueueOutput::stop");
	return unimpErr;
}

OSStatus AudioQueueOutput::pause()
{
	if (verbose) puts("STUB: AudioQueueOutput::pause");
	return unimpErr;
}

OSStatus AudioQueueOutput::reset()
{
	if (verbose) puts("STUB: AudioQueueOutput::reset");
	return unimpErr;
}

OSStatus AudioQueueOutput::dispose(Boolean inImmediate)
{
	if (inImmediate)
	{
		delete this;
		return noErr;
	}
	else
	{
		if (verbose) puts("STUB: AudioQueueOutput::dispose");
		return unimpErr;
	}
}

OSStatus AudioQueueOutput::setOfflineRenderFormat(const AudioStreamBasicDescription *inFormat, const AudioChannelLayout *inLayout)
{
	if (verbose) puts("STUB: AudioQueueOutput::setOfflineRenderFormat");
	return unimpErr;
}

OSStatus AudioQueueOutput::offlineRender(const AudioTimeStamp *inTimestamp, AudioQueueBufferRef ioBuffer, UInt32 inNumberFrames)
{
	if (verbose) puts("STUB: AudioQueueOutput::offlineRender");
	return unimpErr;
}

OSStatus AudioQueueOutput::create(const AudioStreamBasicDescription *inFormat,
		AudioQueueOutputCallback inCallbackProc,
		void *inUserData, CFRunLoopRef inCallbackRunLoop,
		CFStringRef inCallbackRunLoopMode, UInt32 inFlags,
			AudioQueueOutput** newQueue)
{
	// TODO: check arguments
	*newQueue = new AudioQueueOutput(inFormat, inCallbackProc, inUserData,
			inCallbackRunLoop, inCallbackRunLoopMode,
			inFlags);
	return noErr;
}
