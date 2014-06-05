add_executable(LogFile_test LogFile_test.cpp)
target_link_libraries(LogFile_test iak_base)

add_executable(ProcessInfo_test ProcessInfo_test.cpp)
target_link_libraries(ProcessInfo_test iak_base)

add_executable(AsyncLogger_test AsyncLogger_test.cpp)
target_link_libraries(AsyncLogger_test iak_base)

add_executable(Date_test Date_test.cpp)
target_link_libraries(Date_test iak_base)

add_executable(TimeZone_test TimeZone_test.cpp)
target_link_libraries(TimeZone_test iak_base)

