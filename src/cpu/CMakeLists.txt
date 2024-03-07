add_library(
  cpu STATIC
  src/ld.cpp
  src/jmp.cpp
  src/alu.cpp
  src/misc.cpp
  src/cpu_impl.cpp
  src/cpu_impl.hpp
  src/srb.cpp
  src/int.cpp
  src/timer.cpp)

target_include_directories(cpu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(cpu PUBLIC decoder common)

add_subdirectory(ut/big_tests)
