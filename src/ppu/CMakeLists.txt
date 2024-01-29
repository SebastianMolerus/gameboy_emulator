add_library(ppu STATIC src/ppu_impl.cpp src/pixel_fetcher.cpp)

target_include_directories(ppu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(ppu PRIVATE common lcd)

# UT
add_executable(ppu_tests ut/test_ppu.cpp)
target_link_libraries(ppu_tests PRIVATE ppu common lcd GTest::gmock_main
                                        GTest::gmock)
gtest_add_tests(TARGET ppu_tests SOURCES ut/test_ppu.cpp)
