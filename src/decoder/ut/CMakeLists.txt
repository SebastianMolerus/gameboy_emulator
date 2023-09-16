add_executable(decoder_tests test_decoder.cpp)

target_link_libraries(decoder_tests PRIVATE decoder GTest::gtest
                                            GTest::gtest_main)

gtest_add_tests(TARGET decoder_tests)
