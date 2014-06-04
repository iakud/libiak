set(base_SRCS
	Condition.cpp
	Timestamp.cpp
	Thread.cpp
	LogStream.cpp
	Logger.cpp
	LogFile.cpp
	FileUtil.cpp
	ProcessInfo.cpp
	)

add_library(iak_base ${base_SRCS})
target_link_libraries(iak_base pthread rt)
set_target_properties(iak_base PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/iak/base)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()
