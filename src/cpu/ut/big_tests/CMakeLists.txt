set(test_suite_name "big_tests_alu")

add_executable(${test_suite_name} alu/test_alu.cpp)

target_link_libraries(${test_suite_name} PRIVATE cpu GTest::gtest
                                                 GTest::gtest_main assembler)
gtest_add_tests(TARGET ${test_suite_name})

target_compile_definitions(
  ${test_suite_name}
  PRIVATE ALU_TEST_PATH="${CMAKE_CURRENT_LIST_DIR}/sm83-test-data/alu_tests/v1")

target_link_libraries(${test_suite_name} PRIVATE json_spirit cpu)
