add_executable(ppu WIN32 ppu.cpp)

target_link_libraries(ppu PRIVATE wx::core wx::base)
