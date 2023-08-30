add_library(decoder src/decoder.cpp)

# Add Json-Spirit
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
   set_target_properties(json_spirit PROPERTIES
      IMPORTED_LOCATION ${packages_path}/lib/json_spirit.so
      IMPORTED_NO_SONAME TRUE
)
endif(UNIX)

find_path(json_spirit_header json_spirit PATHS ${packages_path})

set_property(TARGET json_spirit PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                                         ${json_spirit_header})

target_link_libraries(decoder PRIVATE json_spirit)

target_include_directories(
  decoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/opcodes.json
     DESTINATION ${CMAKE_BINARY_DIR}/src)

target_compile_definitions(
  decoder PRIVATE OPCODES_JSON_PATH="${CMAKE_BINARY_DIR}/src/opcodes.json")

add_subdirectory(tests)
