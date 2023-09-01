add_executable(arithmetic_tests test_arithmetic.cpp)

target_link_libraries(arithmetic_tests PRIVATE cpu GTest::gtest GTest::gtest_main)

gtest_add_tests(TARGET arithmetic_tests)