cmake_minimum_required(VERSION 2.6)

PROJECT(iak CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CXX_FLAGS
	-g
	-std=c++11
	)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " "
	${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(base)
#add_subdirectory(net)

if (NOT CMAKE_BUILD_NO_EXAMPLES)

endif()
