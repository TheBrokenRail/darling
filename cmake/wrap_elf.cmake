include(darling_lib)
#include(CMakeParseArguments)

function(wrap_elf name elfname)
	add_custom_command(
		OUTPUT
			${CMAKE_CURRENT_BINARY_DIR}/${name}.c
		COMMAND
			${CMAKE_BINARY_DIR}/src/libelfloader/wrapgen/wrapgen
				${elfname} 
				${CMAKE_CURRENT_BINARY_DIR}/${name}.c
				${ARGN}
		DEPENDS
			wrapgen
	)

	set(DYLIB_INSTALL_NAME "/usr/lib/native/lib${name}.dylib")
	include_directories(${CMAKE_SOURCE_DIR}/src/libelfloader/native)
	add_darling_library(${name} SHARED ${CMAKE_CURRENT_BINARY_DIR}/${name}.c)
	target_link_libraries(${name} PRIVATE system elfloader)
	make_fat(${name})
	install(TARGETS ${name} DESTINATION libexec/darling/usr/lib/native)
endfunction(wrap_elf)

function(create_header_dir)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/include)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
endfunction()

function(add_include_dir include_dir file)
    find_path(dir_${include_dir} ${include_dir}/${file})
    if(NOT dir_${include_dir})
        message(FATAL_ERROR "Cannot find ${include_dir} header dir")
    else(NOT dir_${include_dir})
        message(STATUS "Found ${include_dir} include dir: ${dir}/${include_dir}")
    endif(NOT dir_${include_dir})
    file(COPY ${dir_${include_dir}}/${include_dir} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
endfunction(add_include_dir)

