function(create_suite suite_name executable_path)

  set(test_suite_name ${suite_name})

  add_executable(${test_suite_name} ${executable_path})

  target_link_libraries(
    ${test_suite_name} PRIVATE cpu GTest::gtest GTest::gtest_main assembler
                               utils)
  gtest_add_tests(TARGET ${test_suite_name})

  target_link_libraries(${test_suite_name} PRIVATE json_spirit cpu)

endfunction()

# UTILS
add_library(utils STATIC big_tests_utils.cpp)
target_link_libraries(utils PRIVATE cpu GTest::gtest)
target_compile_definitions(
  utils PUBLIC BIG_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/sm83-test-data/")

create_suite(big_tests_alu_add alu/test_add.cpp)
create_suite(big_tests_alu_adc alu/test_adc.cpp)
create_suite(big_tests_alu_sub alu/test_sub.cpp)
create_suite(big_tests_alu_rlc alu/test_rlc.cpp)
create_suite(big_tests_alu_rrc alu/test_rrc.cpp)
create_suite(big_tests_alu_rl alu/test_rl.cpp)
create_suite(big_tests_alu_rr alu/test_rr.cpp)
create_suite(big_tests_alu_sla alu/test_sla.cpp)
create_suite(big_tests_alu_sra alu/test_sra.cpp)
create_suite(big_tests_alu_swap alu/test_swap.cpp)
create_suite(big_tests_alu_srl alu/test_srl.cpp)
create_suite(big_tests_alu_bit alu/test_bit.cpp)
create_suite(big_tests_alu_res alu/test_res.cpp)
create_suite(big_tests_alu_set alu/test_set.cpp)

create_suite(big_tests_cpu_ld_16 cpu/test_ld_16.cpp)
create_suite(big_tests_cpu_ld_8 cpu/test_ld_8.cpp)
create_suite(big_tests_cpu_jump cpu/test_jmp.cpp)
create_suite(big_tests_cpu_add cpu/test_add.cpp)
create_suite(big_tests_cpu_adc cpu/test_adc.cpp)
create_suite(big_tests_cpu_sub cpu/test_sub.cpp)
create_suite(big_tests_cpu_sbc cpu/test_sbc.cpp)
create_suite(big_tests_cpu_and cpu/test_and.cpp)
create_suite(big_tests_cpu_xor cpu/test_xor.cpp)
create_suite(big_tests_cpu_or cpu/test_or.cpp)
create_suite(big_tests_cpu_cp cpu/test_cp.cpp)
create_suite(big_tests_cpu_inc_dec_16 cpu/test_inc_dec_16.cpp)
create_suite(big_tests_cpu_inc_dec_8 cpu/test_inc_dec_8.cpp)
create_suite(big_tests_cpu_other_alu cpu/test_other_alu.cpp)
create_suite(big_tests_cpu_srb cpu/test_srb.cpp)
