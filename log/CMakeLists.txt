set(log_SRCS
	LogStream.cpp
	Logging.cpp
	LogFile.cpp
	AsyncLogger.cpp
	)

add_library(iak_log ${log_SRCS})
target_link_libraries(iak_log iak_base)
set_target_properties(iak_log PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_log DESTINATION lib)

set(HEADERS
  LogStream.h
  Logging.h
  LogFile.h
  AsyncLogger.h
  )

#file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/iak/log)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()