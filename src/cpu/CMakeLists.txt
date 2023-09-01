add_library(cpu STATIC src/cpu.cpp src/instructions/arithmetic.cpp
                       src/instructions/load.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(
  cpu
  PUBLIC decoder
  PRIVATE common)

add_subdirectory(tests)
