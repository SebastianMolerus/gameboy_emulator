add_library(decoder src/decoder.cpp)

find_library(js-var json_spirit)
add_library(json_spirit STATIC IMPORTED)
set_property(TARGET json_spirit PROPERTY IMPORTED_LOCATION ${js-var})

target_link_libraries(decoder PRIVATE restclient-cpp json_spirit)

target_include_directories(
  decoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

add_subdirectory(tests)
