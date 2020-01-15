/*
This file is part of Darling.

Copyright (C) 2015-2016 Lubos Dolezel

Darling is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Darling is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Darling.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <CoreAudio/AudioHardwareImpl.h>
#include <CoreServices/MacErrors.h>
#include <typeinfo>
#include <iostream>

AudioHardwareImpl::AudioHardwareImpl()
{
}


AudioHardwareImpl::~AudioHardwareImpl()
{
}
	
void AudioHardwareImpl::show()
{
	std::cout << typeid(*this).name() << std::endl;
}

OSStatus AudioHardwareImpl::createIOProcID(AudioDeviceIOProc inProc, void* inClientData,
		AudioDeviceIOProcID* outIOProcID)
{
	std::lock_guard<std::mutex> guard(m_procMutex);
	
	if (!outIOProcID || !inProc)
		return paramErr;
	
	*outIOProcID = reinterpret_cast<AudioDeviceIOProcID>(m_nextProcId++);
	m_proc[*outIOProcID] = std::pair<AudioDeviceIOProc, void *>(inProc, inClientData);
	
	return noErr;
}

OSStatus AudioHardwareImpl::destroyIOProcID(AudioDeviceIOProcID inIOProcID)
{
	std::lock_guard<std::mutex> guard(m_procMutex);
	auto it = m_proc.find(inIOProcID);
	
	if (it == m_proc.end())
		return paramErr;
	
	m_proc.erase(it);
	return noErr;
}

OSStatus AudioHardwareImpl::start(AudioDeviceIOProcID inProcID,
		AudioTimeStamp* ioRequestedStartTime, UInt32 inFlags)
{
	AudioHardwareStream* stream;
	
	if (m_streams.find(inProcID) != m_streams.end())
		return noErr;
	
	stream = createStream(inProcID);
	m_streams.insert(std::pair<AudioDeviceIOProcID, std::unique_ptr<AudioHardwareStream>>(inProcID, stream));
	
	// TODO: time
	
	return noErr;
}

OSStatus AudioHardwareImpl::stop(AudioDeviceIOProcID inProcID)
{
	auto it = m_streams.find(inProcID);
	if (it == m_streams.end())
		return kAudioHardwareNotRunningError;
	
	m_streams.erase(it);
	return noErr;
}

static int verbose = 0;

__attribute__((constructor))
static void initme(void) {
    verbose = getenv("STUB_VERBOSE") != NULL;
}

bool AudioHardwareImpl::hasProperty(const AudioObjectPropertyAddress* address) {
    if (verbose) puts("STUB: AudioHardwareImpl::hasProperty");
    return false;
}

OSStatus AudioHardwareImpl::getPropertyDataSize(const AudioObjectPropertyAddress* inAddress, UInt32 inQualifierDataSize, const void* inQualifierData, UInt32* outDataSize) {
    if (verbose) puts("STUB: AudioHardwareImpl::getPropertyDataSize");
    return unimpErr;
}

OSStatus AudioHardwareImpl::isPropertySettable(const AudioObjectPropertyAddress* inAddress, Boolean* outIsSettable) {
    if (verbose) puts("STUB: AudioHardwareImpl::isPropertySettable");
    return unimpErr;
}

OSStatus AudioHardwareImpl::getPropertyData(const AudioObjectPropertyAddress* inAddress, UInt32 inQualifierDataSize, const void* inQualifierData, UInt32* ioDataSize, void* outData) {
    if (verbose) puts("STUB: AudioHardwareImpl::getPropertyData");
    return unimpErr;
}

OSStatus AudioHardwareImpl::setPropertyData(const AudioObjectPropertyAddress* inAddress, UInt32 inQualifierDataSize, const void* inQualifierData, UInt32 inDataSize, const void* inData) {
    if (verbose) puts("STUB: AudioHardwareImpl::setPropertyData");
    return unimpErr;
}

OSStatus AudioHardwareImpl::addPropertyListener(const AudioObjectPropertyAddress* inAddress, AudioObjectPropertyListenerProc inListener, void* inClientData) {
    if (verbose) puts("STUB: AudioHardwareImpl::addPropertyListener");
    return unimpErr;
}

OSStatus AudioHardwareImpl::removePropertyListener(const AudioObjectPropertyAddress* inAddress, AudioObjectPropertyListenerProc inListener, void* inClientData) {
    if (verbose) puts("STUB: AudioHardwareImpl::removePropertyListener");
    return unimpErr;
}
