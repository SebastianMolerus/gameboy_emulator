add_library(translator src/translator.cpp)

target_link_libraries(translator PRIVATE decoder)

find_package(Boost REQUIRED)
target_link_libraries(translator PRIVATE Boost::boost)

target_include_directories(
  translator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

add_subdirectory(ut)
