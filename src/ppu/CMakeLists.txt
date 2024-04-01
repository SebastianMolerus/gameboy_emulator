add_library(ppu STATIC src/ppu_impl.cpp src/pixel_fetcher.cpp src/ppu_modes.cpp)
target_include_directories(ppu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(ppu PRIVATE common lcd)
