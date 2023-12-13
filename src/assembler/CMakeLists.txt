add_library(assembler src/assembler.cpp)

target_link_libraries(assembler PRIVATE decoder Boost::boost)

target_include_directories(
  assembler PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

add_subdirectory(ut)
