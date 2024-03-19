#include "pixel_fetcher.hpp"
#include <cassert>
#include "tile_info.hpp"

namespace
{

rw_device *g_rw;
uint16_t g_bg_tile_map_addr;
uint16_t g_bg_tile_data_addr;

using tile_map_index = size_t;
using tile_addr = uint16_t;
struct screen_coordinates
{
    uint8_t x{};
    uint8_t y{};
};

color get_color_pallete_based(uint8_t id)
{
    constexpr color WHITE{1.f, 1.f, 1.f};
    constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
    constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
    constexpr color BLACK{};

    uint8_t const bg_palette = g_rw->read(0xFF47, device::PPU);

    uint8_t val{};
    switch (id)
    {
    case 3:
        if (checkbit(bg_palette, 7))
            setbit(val, 1);
        if (checkbit(bg_palette, 6))
            setbit(val, 0);
        break;
    case 2:
        if (checkbit(bg_palette, 5))
            setbit(val, 1);
        if (checkbit(bg_palette, 4))
            setbit(val, 0);
        break;
    case 1:
        if (checkbit(bg_palette, 3))
            setbit(val, 1);
        if (checkbit(bg_palette, 2))
            setbit(val, 0);
        break;
    case 0:
        if (checkbit(bg_palette, 1))
            setbit(val, 1);
        if (checkbit(bg_palette, 0))
            setbit(val, 0);
        break;
    default:
        assert(false);
    }

    if (val == 0)
        return WHITE;
    else if (val == 1)
        return LIGHT_GRAY;
    else if (val == 2)
        return DARK_GRAY;
    else
        return BLACK;
}

constexpr std::array<color, 8> convert_tile_line_to_colors(uint16_t line)
{
    uint8_t const l = line >> 8; // a b c d ...
    uint8_t const r = line;      // i j k l ...

    // result[i] = ia jb kc
    std::array<uint8_t, 8> result{};
    for (int i = 0; i < 8; ++i)
    {
        uint8_t id{};
        if (checkbit(r, 7 - i))
            setbit(id, 1);
        if (checkbit(l, 7 - i))
            setbit(id, 0);
        result[i] = id;
    }

    std::array<color, 8> colors;
    for (int i = 0; i < 8; ++i)
        colors[i] = get_color_pallete_based(result[i]);
    return colors;
}

class fetcher
{
  public:
    fetcher(uint16_t tile_map_addr, uint16_t tile_data_addr) : m_tile_map_addr{tile_map_addr}, m_tile_data_addr{tile_data_addr}
    {
    }

    uint16_t get_pixel_line(screen_coordinates sc)
    {
        tile_map_index tmi = map_screen_coordinates_to_tile_map({sc.x, sc.y});
        tile_addr ta = get_tile_addr(tmi);

        constexpr uint16_t TILE_LINE_SIZE_B{2};
        auto tile_line_addr = ((sc.y % 8) * TILE_LINE_SIZE_B) + ta;

        uint8_t const tile_lo = g_rw->read(tile_line_addr++, device::PPU);
        uint8_t const tile_hi = g_rw->read(tile_line_addr, device::PPU);

        uint16_t line = tile_hi;
        line <<= 8;
        line |= tile_lo;
        return line;
    }

  private:
    uint16_t const m_tile_map_addr;
    uint16_t const m_tile_data_addr;

    // Convert tile index into address where it is stored in memory
    tile_addr get_tile_addr(tile_map_index tmi)
    {
        constexpr uint16_t TILE_SIZE_B{16};
        auto const tile_index = g_rw->read(tmi + g_bg_tile_map_addr, device::PPU);
        return tile_index * TILE_SIZE_B + m_tile_data_addr;
    }

    // Returns tile_map_index which can be a value from 0 to 1023 ( 32 rows x 32 columns of 1B tiles indexes )
    tile_map_index map_screen_coordinates_to_tile_map(screen_coordinates sc)
    {
        constexpr size_t TILES_IN_TILEMAP_ROW{32};
        return static_cast<int>(sc.x / 8.0f) + static_cast<int>(sc.y / 8.0f) * TILES_IN_TILEMAP_ROW;
    }
};

} // namespace

pixel_fetcher::pixel_fetcher(rw_device &rw_device, drawing_device &dd) : rw{rw_device}, dd{dd}
{
    g_rw = &(this->rw);
}

pixel_fetcher::LINE_DRAWING_STATUS pixel_fetcher::dot(uint8_t screen_line, std::array<sprite, 10> const &visible_sprites)
{
    switch (dot_counter)
    {
    case 0: {
        fetcher f{g_bg_tile_map_addr, g_bg_tile_data_addr};
        auto const pixel_line = f.get_pixel_line({m_current_x, static_cast<uint8_t>(y_scroll + screen_line)});
        pixels = convert_tile_line_to_colors(pixel_line);
    }
    break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        break;
    case 7:
        if (pixel_fifo.size() == 8 || pixel_fifo.empty())
        {
            for (int i = 0; i < 8; ++i)
                pixel_fifo.push(pixels[i]);

            dot_counter = -1;
            m_current_x += 8;
        }
        else
            dot_counter = 6;
        break;
    }

    if (pixel_fifo.size() > 8)
    {
        ++pushed_px;
        color c = pixel_fifo.front();
        pixel_fifo.pop();

        dd.push_pixel(c);
    }

    ++dot_counter;
    if (pushed_px == 160)
    {
        m_current_x = dot_counter = 0;
        std::queue<color> empty;
        swap(pixel_fifo, empty);
        pushed_px = 0;
        return LINE_DRAWING_STATUS::DONE;
    }
    else
        return LINE_DRAWING_STATUS::IN_PROGRESS;
}

void pixel_fetcher::prepare_for_draw_one_line()
{
    constexpr uint16_t LCD_CTRL_addr{0xFF40};
    uint8_t const lcd_ctrl = rw.read(LCD_CTRL_addr, device::PPU);
    g_bg_tile_map_addr = checkbit(lcd_ctrl, 3) ? 0x9C00 : 0x9800;
    g_bg_tile_data_addr = checkbit(lcd_ctrl, 4) ? 0x8000 : 0x8800;

    constexpr uint16_t scroll_y_addr{0xFF42};
    constexpr uint16_t scroll_x_addr{0xFF43};
    y_scroll = rw.read(scroll_y_addr, device::PPU);
    x_scroll = rw.read(scroll_x_addr, device::PPU);
}
