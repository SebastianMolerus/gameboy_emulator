add_library(Cpu STATIC cpu.cpp)
target_include_directories(Cpu INTERFACE ${CMAKE_CURRENT_LIST_DIR})