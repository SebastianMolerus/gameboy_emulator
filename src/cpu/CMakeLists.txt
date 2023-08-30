add_library(cpu STATIC src/cpu.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(cpu PRIVATE decoder common)

add_subdirectory(tests)
