add_library(cpu STATIC src/cpu.cpp src/ld.cpp src/jmp.cpp src/arith.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(cpu PUBLIC decoder)

add_subdirectory(ut)
