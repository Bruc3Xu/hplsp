add_executable(test_loop test_loop.cc)
target_link_libraries(test_loop PRIVATE netbridge)

add_executable(test_loop2 test_loop2.cc)
target_link_libraries(test_loop2 PRIVATE netbridge)