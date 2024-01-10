cmake_minimum_required(VERSION 3.24)
project(RM_GB_Emu)

enable_testing()

include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Git REQUIRED)

message(STATUS "Submodule update in Progress")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init)

# Json Spirit
add_library(json_spirit SHARED IMPORTED)

set(packages_path ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})

if(NOT EXISTS ${packages_path})
  message(FATAL_ERROR "Vcpkg dependecy path doesn't exist.")
endif()

if(WIN32)
  set_property(TARGET json_spirit PROPERTY IMPORTED_IMPLIB
                                           ${packages_path}/lib/json_spirit.lib)
  set_property(TARGET json_spirit PROPERTY IMPORTED_LOCATION
                                           ${packages_path}/bin/json_spirit.dll)
endif(WIN32)

if(UNIX)
  set_property(
    TARGET json_spirit PROPERTY IMPORTED_LOCATION
                                ${packages_path}/lib/libjson_spirit.so)
endif(UNIX)

find_path(json_spirit_header json_spirit PATHS ${packages_path})

set_property(TARGET json_spirit PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                         ${json_spirit_header})
# ---------------------------------

add_subdirectory(src)

add_executable(RM_GB_Emu_App src/main.cpp src/load.cpp src/print.cpp)

target_compile_definitions(
  RM_GB_Emu_App
  PRIVATE BOOT_ROM_FILE="${CMAKE_CURRENT_LIST_DIR}/src/res/dmg_boot_rom.gb"
          ROM_FILE="${CMAKE_CURRENT_LIST_DIR}/src/res/TetrisJUEV1.1.gb")

target_link_libraries(RM_GB_Emu_App PRIVATE decoder cpu)
