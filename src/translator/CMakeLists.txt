add_library(translator src/translator.cpp)

target_link_libraries(translator PRIVATE decoder)

target_include_directories(
  translator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

add_subdirectory(tests)
