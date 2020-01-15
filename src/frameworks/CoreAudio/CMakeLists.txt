project(CoreAudio)

set(DYLIB_COMPAT_VERSION "1.0.0")
set(DYLIB_CURRENT_VERSION "1.0.0")

add_framework(CoreAudio
    FAT
    CURRENT_VERSION
    VERSION "A"

    SOURCES
        src/AudioHardware.cpp
        src/AudioHardwareImpl.cpp
        src/AudioHardwareStream.cpp

    DEPENDENCIES
        system
        objc
        cxx
        Foundation
        CoreServices
)
