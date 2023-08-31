add_executable(cpu_data_tests test_cpu_data.cpp)

target_link_libraries(cpu_data_tests PRIVATE cpu GTest::gtest GTest::gtest_main)

gtest_add_tests(TARGET cpu_data_tests)
