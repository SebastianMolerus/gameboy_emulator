add_executable(cpu_data_tests test_cpu_data.cpp)
target_link_libraries(cpu_data_tests PRIVATE cpu GTest::gtest GTest::gtest_main)
gtest_add_tests(TARGET cpu_data_tests)

add_executable(load_tests test_new_approach.cpp utils.h)
target_link_libraries(load_tests PRIVATE cpu GTest::gtest GTest::gtest_main
                                         translator)
gtest_add_tests(TARGET load_tests)
