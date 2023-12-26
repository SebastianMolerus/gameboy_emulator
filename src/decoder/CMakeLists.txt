add_library(decoder src/decoder.cpp)

target_link_libraries(decoder PRIVATE json_spirit)

target_include_directories(
  decoder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/opcodes.json
     DESTINATION ${CMAKE_BINARY_DIR}/src)

target_compile_definitions(
  decoder PRIVATE OPCODES_JSON_PATH="${CMAKE_BINARY_DIR}/src/opcodes.json")

add_subdirectory(ut)
