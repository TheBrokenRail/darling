project(AudioToolbox)

include(wrap_elf)

create_header_dir()

add_include_dir(libavutil avutil.h)
add_include_dir(libavcodec avcodec.h)
add_include_dir(libavresample avresample.h)

wrap_elf(avutil libavutil.so)
wrap_elf(avcodec libavcodec.so)
wrap_elf(avresample libavresample.so)

set(DYLIB_COMPAT_VERSION "1.0.0")
set(DYLIB_CURRENT_VERSION "1.0.0")

add_framework(AudioToolbox
	FAT
	CURRENT_VERSION
	VERSION "A"

	SOURCES
		src/AudioConverter.cpp
	    src/AudioConverterImpl.cpp
	    src/AudioFile.cpp

	DEPENDENCIES
		system
		cxx
		CoreServices
		avutil
		avcodec
		avresample
)
