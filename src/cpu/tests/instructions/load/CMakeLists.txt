add_executable(load_tests test_load.cpp test_ld_16.cpp program_creator.cpp)

target_link_libraries(load_tests PRIVATE cpu GTest::gtest GTest::gtest_main
                                         translator)

gtest_add_tests(TARGET load_tests)
