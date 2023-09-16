add_executable(translator_test test_translator.cpp)

target_link_libraries(translator_test PRIVATE decoder GTest::gtest
                                              GTest::gtest_main translator)

gtest_add_tests(TARGET translator_test)
