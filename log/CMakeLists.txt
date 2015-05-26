set(log_SRCS
	Logging.cpp
	LogStream.cpp
	LogMessage.cpp
	Logger.cpp
	AsyncLogging.cpp
	AsyncLogger.cpp
	)

add_library(iak_log ${log_SRCS})
target_link_libraries(iak_log iak_base)
set_target_properties(iak_log PROPERTIES COMPILE_FLAGS "-std=c++14")

install(TARGETS iak_log DESTINATION lib)

set(HEADERS
	Logging.h
	LogStream.h
	LogMessage.h
	Logger.h
	AsyncLogger.h
	)

install(FILES ${HEADERS} DESTINATION include/iak/log)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()