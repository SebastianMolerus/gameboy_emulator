add_library(cpu STATIC src/cpu.cpp src/arithmetic.cpp src/ld.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(
  cpu
  PUBLIC decoder
  PRIVATE common)

add_subdirectory(ut)
