add_executable(assembler_test test_assembler.cpp)

target_link_libraries(assembler_test PRIVATE decoder GTest::gtest
                                             GTest::gtest_main assembler)

gtest_add_tests(TARGET assembler_test)
