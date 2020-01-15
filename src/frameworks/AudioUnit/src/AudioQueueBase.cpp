#include <AudioUnit/AudioQueueBase.h>
#include <CoreServices/MacErrors.h>
#include <cstdlib>
#include <cstdio>

AudioQueue::AudioQueue(const AudioStreamBasicDescription* format, void* userData,
			CFRunLoopRef runloop, CFStringRef runloopMode, UInt32 flags)
: m_format(*format), m_userData(userData), m_flags(flags)
{
	m_runloop = (CFRunLoopRef) CFRetain(runloop);
	m_runloopMode = (CFStringRef) CFRetain(runloopMode);
}

AudioQueue::~AudioQueue()
{
	CFRelease(m_runloop);
	CFRelease(m_runloopMode);
}

static int verbose = 0;

__attribute__((constructor))
static void initme(void) {
    verbose = getenv("STUB_VERBOSE") != NULL;
}

OSStatus AudioQueue::getParameter(AudioQueueParameterID inParamID, AudioQueueParameterValue *outValue)
{
	if (verbose) puts("STUB: AudioQueue::getParameter");
	return unimpErr;
}

OSStatus AudioQueue::setParameter(AudioQueueParameterID inParamID, AudioQueueParameterValue inValue)
{
	if (verbose) puts("STUB: AudioQueue::setParameter");
	return unimpErr;
}

OSStatus AudioQueue::getProperty(AudioQueuePropertyID inID, void *outData, UInt32 *ioDataSize)
{
	if (verbose) puts("STUB: AudioQueue::getProperty");
	return unimpErr;
}

OSStatus AudioQueue::setProperty(AudioQueuePropertyID inID, const void *inData, UInt32 inDataSize)
{
	if (verbose) puts("STUB: AudioQueue::setProperty");
	return unimpErr;
}

OSStatus AudioQueue::getPropertySize(AudioQueuePropertyID inID, UInt32 *outDataSize)
{
	if (verbose) puts("STUB: AudioQueue::getPropertySize");
	return unimpErr;
}

OSStatus AudioQueue::addPropertyListener(AudioQueuePropertyID inID, AudioQueuePropertyListenerProc inProc, void *inUserData)
{
	if (verbose) puts("STUB: AudioQueue::addPropertyListener");
	return unimpErr;
}

OSStatus AudioQueue::removePropertyListener(AudioQueuePropertyID inID, AudioQueuePropertyListenerProc inProc, void *inUserData)
{
	if (verbose) puts("STUB: AudioQueue::removePropertyListener");
	return unimpErr;
}
