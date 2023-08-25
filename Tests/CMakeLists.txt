enable_testing()

add_executable(ut_demo ut_demo.cpp)

target_link_libraries(ut_demo PRIVATE GTest::gtest GTest::gtest_main
                                      GTest::gmock GTest::gmock_main)

add_test(demo_gtests ut_demo)
