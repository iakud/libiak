set(net_SRCS
	Watcher.cpp
	EventLoop.cpp
	EPollLoop.cpp
	InetAddress.cpp
	Connector.cpp
	Acceptor.cpp
	Packet.cpp
	TcpConnection.cpp
	EventLoopThreadPool.cpp
	TcpClient.cpp
	TcpServer.cpp
	)

add_library(iak_net ${net_SRCS})
target_link_libraries(iak_net iak_base)
set_target_properties(iak_net PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_net DESTINATION lib)

set(HEADERS
  EventLoop.h
  InetAddress.h
  )

install(FILES ${HEADERS} DESTINATION include/iak/net)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
	add_subdirectory(tests)
endif()
