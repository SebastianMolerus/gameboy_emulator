add_executable(common_tests test_common.cpp)

target_link_libraries(common_tests PRIVATE decoder GTest::gtest
                                           GTest::gtest_main)

gtest_add_tests(TARGET common_tests)
