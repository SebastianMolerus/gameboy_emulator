add_executable(cpu_tests test_cpu.cpp)

target_link_libraries(cpu_tests PRIVATE cpu GTest::gtest GTest::gtest_main)

gtest_add_tests(TARGET cpu_tests)
