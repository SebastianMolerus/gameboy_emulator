add_library(cpu STATIC src/cpu.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(tests)