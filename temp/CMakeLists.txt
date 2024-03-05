add_executable(child_learn child_learn.cpp)

target_link_libraries(child_learn PRIVATE glm::glm glfw glad::glad)
