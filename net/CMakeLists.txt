set(net_SRCS

	)

add_library(iak_net ${net_SRCS})
target_link_libraries(iak_net iak_base)
set_target_properties(iak_base PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS iak_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/iak/net)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
#	add_subdirectory(tests)
endif()
