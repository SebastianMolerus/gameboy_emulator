cmake_minimum_required(VERSION 3.24)
project(RM_GB_Emu)

enable_testing()

include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

find_package(Boost REQUIRED)

# Data for big tests include(FetchContent) FetchContent_Declare( sm83-test-data
# GIT_REPOSITORY "https://github.com/adtennant/sm83-test-data.git" GIT_TAG
# "origin/master" SOURCE_DIR
# "${CMAKE_CURRENT_LIST_DIR}/src/cpu/ut/big_tests/sm83-test-data")
# FetchContent_MakeAvailable(sm83-test-data)

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

add_executable(RM_GB_Emu_App src/main.cpp)
