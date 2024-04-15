#include "pixel_fetcher.hpp"

namespace
{

rw_device *g_rw;

uint16_t g_background_map_addr;
uint16_t g_background_data_addr;

uint16_t g_window_map_addr;
uint16_t g_window_data_addr;

uint8_t g_sprite_height;

using tile_map_index = size_t;
using addr = uint16_t;

tile_map_index map_screen_coordinates_to_tile_map(screen_coordinates sc)
{
    constexpr size_t TILES_IN_TILEMAP_ROW{32};
    return static_cast<int>(sc.m_x / 8.0f) + static_cast<int>(sc.m_y / 8.0f) * TILES_IN_TILEMAP_ROW;
}

constexpr uint16_t TILE_SIZE_B{16};
constexpr uint16_t TILE_LINE_SIZE_B{2};

addr get_background_addr(tile_map_index tmi)
{
    auto const tile_index = g_rw->read(tmi + g_background_map_addr, device::PPU);
    return tile_index * TILE_SIZE_B + g_background_data_addr;
}

addr get_window_addr(tile_map_index tmi)
{
    auto const tile_index = g_rw->read(tmi + g_window_map_addr, device::PPU);
    return tile_index * TILE_SIZE_B + g_window_data_addr;
}

uint16_t read_two_bytes(addr a)
{
    uint8_t const tile_lo = g_rw->read(a, device::PPU);
    uint8_t const tile_hi = g_rw->read(a + 1, device::PPU);
    uint16_t line = tile_hi;
    line <<= 8;
    line |= tile_lo;
    return line;
}

uint16_t get_background(screen_coordinates sc)
{
    tile_map_index const tmi = map_screen_coordinates_to_tile_map(sc);
    addr const addr = get_background_addr(tmi);
    auto bg_tile_line_addr = ((sc.m_y % 8) * TILE_LINE_SIZE_B) + addr;
    return read_two_bytes(bg_tile_line_addr);
}

uint16_t get_window(screen_coordinates sc)
{
    tile_map_index const tmi = map_screen_coordinates_to_tile_map(sc);
    addr const addr = get_window_addr(tmi);
    auto window_tile_line_addr = ((sc.m_y % 8) * TILE_LINE_SIZE_B) + addr;
    return read_two_bytes(window_tile_line_addr);
}

} // namespace

pixel_fetcher::pixel_fetcher(rw_device &rw_device) : m_rw{rw_device}
{
    g_rw = &m_rw;
    update_addresses();
}

uint16_t pixel_fetcher::fetch_tile_line(screen_coordinates sc)
{
    return get_background(sc);
}

void pixel_fetcher::update_addresses()
{
    constexpr uint16_t LCD_CTRL_addr{0xFF40};
    uint8_t const lcd_ctrl = m_rw.read(LCD_CTRL_addr, device::PPU);
    g_background_map_addr = checkbit(lcd_ctrl, 3) ? 0x9C00 : 0x9800;
    g_background_data_addr = checkbit(lcd_ctrl, 4) ? 0x8000 : 0x8800;

    g_window_map_addr = checkbit(lcd_ctrl, 6) ? 0x9C00 : 0x9800;
    g_window_data_addr = g_background_data_addr;

    g_sprite_height = checkbit(lcd_ctrl, 2) ? 16 : 8;
}
