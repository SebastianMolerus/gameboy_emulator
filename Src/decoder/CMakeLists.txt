add_executable(decoder src/main.cpp)

target_link_libraries(decoder PRIVATE restclient-cpp)
