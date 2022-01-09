cmake_minimum_required(VERSION 3.22)

#project name
project(regrex_lex VERSION 1.1)


SET( CMAKE_VERBOSE_MAKEFILE on )

#static link 
set(BUILD_SHARED_LIBS OFF)
if(MSVC)
add_compile_options(
	$<$<CONFIG:>:/MT>
	$<$<CONFIG:Debug>:/MTd>
	$<$<CONFIG:Release>:/MT>
)
else()
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

#head file path
include_directories(src)



#source directory
aux_source_directory(src SRC_LISTA)

add_executable(regrex_lex ${SRC_LISTA})





