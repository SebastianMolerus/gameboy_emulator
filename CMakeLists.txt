cmake_minimum_required(VERSION 3.24)
project(RM_GB_Emu)

enable_testing()

include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_subdirectory(src)

add_executable(RM_GB_Emu_App main.cpp)

target_link_libraries(RM_GB_Emu_App PRIVATE cartridge_loader cpu)