add_executable(cpu_data_tests test_cpu_data.cpp)
target_link_libraries(cpu_data_tests PRIVATE cpu GTest::gtest GTest::gtest_main)
gtest_add_tests(TARGET cpu_data_tests)

add_executable(load_tests test_ld_8.cpp test_ld_16.cpp)
target_link_libraries(load_tests PRIVATE cpu GTest::gtest GTest::gtest_main
                                         translator)
gtest_add_tests(TARGET load_tests)

add_executable(ctrl_tests test_ctrl.cpp)
target_link_libraries(ctrl_tests PRIVATE cpu GTest::gtest GTest::gtest_main
                                         translator)
gtest_add_tests(TARGET ctrl_tests)
