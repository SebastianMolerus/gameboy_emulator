add_library(ppu STATIC src/ppu_impl.cpp src/lcd_ctrl.cpp)

target_include_directories(ppu PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(ppu PRIVATE common lcd)
