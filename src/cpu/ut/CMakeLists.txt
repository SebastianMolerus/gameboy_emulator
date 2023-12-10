add_executable(load_tests test_ld_16.cpp test_ld_8.cpp)

target_include_directories(load_tests PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(load_tests PRIVATE cpu GTest::gtest GTest::gtest_main
                                         translator)
gtest_add_tests(TARGET load_tests)
