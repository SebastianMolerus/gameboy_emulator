function(create_suite suite_name executable_path)

  set(test_suite_name ${suite_name})

  add_executable(${test_suite_name} ${executable_path})

  target_link_libraries(${test_suite_name} PRIVATE cpu GTest::gtest
                                                   GTest::gtest_main assembler)
  gtest_add_tests(TARGET ${test_suite_name})

  target_link_libraries(${test_suite_name} PRIVATE json_spirit cpu)

  target_compile_definitions(
    ${test_suite_name}
    PRIVATE BIG_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/sm83-test-data/")

endfunction()

create_suite(big_tests_adc alu/test_adc.cpp)
create_suite(big_tests_add alu/test_add.cpp)
create_suite(big_tests_sub alu/test_sub.cpp)

create_suite(big_tests_f8 cpu/test_f8.cpp)
