set(test_suite_name "cpu_tests")

add_executable(${test_suite_name} test_arith.cpp test_jmp.cpp test_ld_16.cpp
                                  test_ld_8.cpp)

target_include_directories(${test_suite_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${test_suite_name} PRIVATE cpu GTest::gtest
                                                 GTest::gtest_main assembler)
gtest_add_tests(TARGET ${test_suite_name})
