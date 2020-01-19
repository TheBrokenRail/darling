project(AudioUnit)

cmake_minimum_required(VERSION 2.4.0)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

option(ENABLE_ALSA "Enable ALSA audio" ON)
option(ENABLE_PULSEAUDIO "Enable PulseAudio" ON)

include(wrap_elf)

create_header_dir()

if(ENABLE_ALSA)
    add_include_dir(alsa asoundlib.h)
endif(ENABLE_ALSA)

if(ENABLE_PULSEAUDIO)
    add_include_dir(pulse pulseaudio.h)
endif(ENABLE_PULSEAUDIO)

set(AudioUnit_SRCS
	src/AUComponent.cpp
	src/AUGraph.cpp
	src/AudioUnit.cpp
	src/AudioOutputUnitComponent.cpp
	src/AudioUnitBase.cpp
	src/AudioUnitRenderer.cpp

	src/AudioQueue.cpp
	src/AudioQueueBase.cpp
	src/AudioQueueOutput.cpp
)

if(ENABLE_ALSA)
    add_definitions(-DENABLE_ALSA)
    list(APPEND AudioUnit_SRCS src/AudioUnitALSA.cpp)
    list(APPEND SoundLibs "asound")
    wrap_elf(asound libasound.so)
endif(ENABLE_ALSA)

if(ENABLE_PULSEAUDIO)
    add_definitions(-DENABLE_PULSEAUDIO)
    list(APPEND AudioUnit_SRCS src/AudioUnitPA.cpp)
    list(APPEND SoundLibs "pulse")

    find_library(PULSE_LIB pulseaudio/libpulsecommon-13.0.so)
    if(PULSE_LIB)
        wrap_elf(pulse libpulse.so ${PULSE_LIB})
    else(PULSE_LIB)
        wrap_elf(pulse libpulse.so)
    endif(PULSE_LIB)
endif(ENABLE_PULSEAUDIO)

set(DYLIB_COMPAT_VERSION "1.0.0")
set(DYLIB_CURRENT_VERSION "1.0.0")

add_framework(AudioUnit
    FAT
    CURRENT_VERSION
    VERSION "A"

    SOURCES
        ${AudioUnit_SRCS}

    DEPENDENCIES
        system
        system_pthread
        cxx
        objc
        libdispatch_shared
        CoreFoundation
        ${SoundLibs}

    CIRCULAR_DEPENDENCIES
        CoreServices
)
