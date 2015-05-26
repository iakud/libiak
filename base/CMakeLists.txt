cmake_minimum_required(VERSION 2.6)

set(base_SRCS
	FileUtil.cpp
	ProcessInfo.cpp
	Date.cpp
	TimeZone.cpp
	)

add_library(iak_base ${base_SRCS})
set_target_properties(iak_base PROPERTIES COMPILE_FLAGS "-std=c++14")

install(TARGETS iak_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/iak/base)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()
