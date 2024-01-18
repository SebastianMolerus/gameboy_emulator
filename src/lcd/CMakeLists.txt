add_library(lcd STATIC lcd.cpp)

target_link_libraries(lcd PRIVATE glfw glad::glad glm::glm common)

target_include_directories(
  lcd PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
