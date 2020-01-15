#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioUnitALSA.h>
#include <AudioUnit/AudioUnitPA.h>
#include <alsa/asoundlib.h>
#include <CoreServices/MacErrors.h>

AudioComponent AudioComponentFindNext(AudioComponent inAComponent, AudioComponentDescription *inDesc)
{
	int index = -1;

	if (inDesc->componentType != kAudioUnitType_Output && inDesc->componentType != kAudioUnitType_Mixer)
		return nullptr;

	if (inAComponent != nullptr)
		index = GetComponentIndex(inAComponent);

	if (snd_card_next(&index) || index == -1)
		return nullptr;

	return CreateComponent(kComponentTypeAudioUnit, index);
}

OSStatus AudioComponentGetDescription(AudioComponent inComponent, AudioComponentDescription *outDesc)
{
	if (!outDesc)
		return paramErr;
	
	memset(outDesc, 0, sizeof(*outDesc));
	outDesc->componentType = kAudioUnitType_Output;
	
	return noErr;
}

OSStatus AudioComponentGetVersion(AudioComponent inComponent, UInt32 *outVersion)
{
	*outVersion = 0;
	return noErr;
}

Boolean AudioComponentInstanceCanDo(AudioComponentInstance inInstance, SInt16 inSelectorID)
{
	return true;
}

OSStatus AudioComponentInstanceDispose(AudioComponentInstance inInstance)
{
	delete inInstance;
	return noErr;
}

AudioComponent AudioComponentInstanceGetComponent(AudioComponentInstance inInstance)
{
	return CreateComponent(kComponentTypeAudioUnit, static_cast<AudioOutputUnitComponent*>(inInstance)->cardIndex());
}

OSStatus AudioComponentInstanceNew(AudioComponent inComponent, AudioComponentInstance *outInstance)
{
#if defined(ENABLE_PULSEAUDIO)
	*outInstance = new AudioUnitPA;
#elif defined(ENABLE_ALSA)
	*outInstance = AudioUnitALSA::create(GetComponentIndex(inComponent));
#else
	*outInstance = nullptr;
	return unimpErr;
#endif
	return *outInstance ? noErr : paramErr;
}

OSStatus AudioComponentCopyName(AudioComponent inComponent, CFStringRef *outName)
{
	int index = GetComponentIndex(inComponent);
	char* name;
	
	if (!outName)
		return paramErr;
	
	if (snd_card_get_longname(index, &name))
		return paramErr;
	
	*outName = CFStringCreateWithCString(nullptr, name, kCFStringEncodingUTF8);
	free(name);
	
	return noErr;
}

UInt32 AudioComponentCount(AudioComponentDescription *inDesc)
{
	UInt32 count = 0;
	int index = -1;
	
	// Is there a better way?
	do
	{
		snd_card_next(&index);
		if (index != -1)
			count++;
	}
	while (index != -1);
	
	return count;
}

