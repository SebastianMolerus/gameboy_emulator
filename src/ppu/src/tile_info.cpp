#include "tile_info.hpp"

namespace
{

constexpr uint16_t LCD_CTRL_addr{0xFF40};
constexpr uint16_t TILE_SIZE_B{16};
constexpr uint16_t TILE_LINE_SIZE_B{2};
constexpr size_t TILES_IN_TILEMAP_ROW{32};

} // namespace

tile_info::tile_info(rw_device &r) : rw{r}
{
}

uint16_t tile_info::get_tile_line_addr(uint8_t x, uint8_t y)
{
    auto const tile_offset = get_tile_offset(x, y);

    auto const tile_index = rw.read(tile_offset + get_tile_map_addr(), device::PPU);

    auto const tile_addr = tile_index * TILE_SIZE_B + get_tile_data_addr();

    return ((y % 8) * TILE_LINE_SIZE_B) + tile_addr;
}

uint16_t tile_info::get_tile_map_addr()
{
    uint8_t const lcd_ctrl = rw.read(LCD_CTRL_addr, device::PPU);
    if (checkbit(lcd_ctrl, 3))
        return 0x9C00;
    else
        return 0x9800;
}

uint16_t tile_info::get_tile_data_addr()
{
    uint8_t const lcd_ctrl = rw.read(LCD_CTRL_addr, device::PPU);
    if (checkbit(lcd_ctrl, 4))
        return 0x8000;
    else
        return 0x8800;
}

size_t tile_info::get_tile_offset(uint8_t x, uint8_t y)
{
    return x / 8.0f + static_cast<int>(y / 8.0f) * TILES_IN_TILEMAP_ROW;
}
