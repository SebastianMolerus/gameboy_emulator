add_library(cartridge_loader STATIC src/cartridge_loader.cpp)
target_include_directories(cartridge_loader PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)