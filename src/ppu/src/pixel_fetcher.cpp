#include "pixel_fetcher.hpp"
#include <cassert>

namespace
{
constexpr uint16_t SCX_addr{0xFF43};
constexpr uint16_t SCY_addr{0xFF42};
constexpr uint16_t LCD_CTRL_addr{0xFF40};

uint16_t bg_tile_map_addr{};
uint16_t tile_data_addr{};

void update_addr(rw_device &r)
{
    uint8_t const lcd_ctrl = r.read(LCD_CTRL_addr, device::PPU);
    if (checkbit(lcd_ctrl, 3))
        bg_tile_map_addr = 0x9800;
    else
        bg_tile_map_addr = 0x9C00;

    if (checkbit(lcd_ctrl, 4))
        tile_data_addr = 0x8000;
    else
        tile_data_addr = 0x8800;
}

} // namespace

size_t get_tile_offset(int pix_x, int pix_y)
{
    assert(pix_x >= 0 && pix_x <= 255);
    assert(pix_y >= 0 && pix_y <= 255);
    return pix_x / 8.0f + static_cast<int>(pix_y / 8.0f) * 32;
}

size_t get_tile_data_offset(int pix_x, int pix_y)
{
    return (pix_x % 8) * 2 + (2 * pix_y);
}

uint8_t get_tile_line(rw_device &r, int pix_x, int pix_y)
{
    update_addr(r);

    size_t const tile_offset{get_tile_offset(pix_x, pix_y)};
    size_t const tile_index{r.read(bg_tile_map_addr + tile_offset, device::PPU)};

    constexpr size_t tile_size{16};
    return r.read(get_tile_data_offset(pix_x, pix_y) + tile_index * tile_size, device::PPU);
}

pixel_fetcher::pixel_fetcher(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
}

bool pixel_fetcher::dot()
{
    uint8_t tile_line = get_tile_line(m_rw_device, 0, 0);

    return true;
}
