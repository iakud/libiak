set(frame_SRCS
	AsyncNet.cpp
	NetFrame.cpp
	DataSocket.cpp
	ListenSocket.cpp
	ConnecSocket.cpp
	)

add_library(iak_frame ${frame_SRCS})
target_link_libraries(iak_frame iak_base iak_net)
set_target_properties(iak_frame PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_frame DESTINATION lib)

set(HEADERS
	NetFrame.h
	DataSocket.h
	ListenSocket.h
	ConnecSocket.h
	)

install(FILES ${HEADERS} DESTINATION include/iak/frame)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	#add_subdirectory(tests)
endif()