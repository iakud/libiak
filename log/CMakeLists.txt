set(log_SRCS
	LogStream.cpp
	Logger.cpp
	LogFile.cpp
	AsyncLogging.cpp
	)

add_library(iak_log ${log_SRCS})
target_link_libraries(iak_log iak_base)
set_target_properties(iak_log PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_log DESTINATION lib)

set(HEADERS
  LogStream.h
  Logging.h
  LogFile.h
  AsyncLogging.h
  )

#file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/iak/log)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()
