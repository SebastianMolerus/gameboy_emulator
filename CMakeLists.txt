cmake_minimum_required(VERSION 3.24)
project(RM_GB_Emu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory(Src)
add_subdirectory(Tests)

add_executable(RM_GB_Emu_App main.cpp)