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
        bg_tile_map_addr = 0x9C00;
    else
        bg_tile_map_addr = 0x9800;

    if (checkbit(lcd_ctrl, 4))
        tile_data_addr = 0x8000;
    else
        tile_data_addr = 0x8800;
}

color get_color_pallete_based(uint8_t id)
{
    constexpr color WHITE{1.f, 1.f, 1.f};
    constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
    constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
    constexpr color BLACK{};

    auto BGP = 0xfc;

    uint8_t val{};
    switch (id)
    {
    case 3:
        if (checkbit(BGP, 7))
            setbit(val, 1);
        if (checkbit(BGP, 6))
            setbit(val, 0);
        break;
    case 2:
        if (checkbit(BGP, 5))
            setbit(val, 1);
        if (checkbit(BGP, 4))
            setbit(val, 0);
        break;
    case 1:
        if (checkbit(BGP, 3))
            setbit(val, 1);
        if (checkbit(BGP, 2))
            setbit(val, 0);
        break;
    case 0:
        if (checkbit(BGP, 1))
            setbit(val, 1);
        if (checkbit(BGP, 0))
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

} // namespace

std::array<uint8_t, 8> convert_line_to_ids(uint16_t line)
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
    return result;
}

size_t get_tile_offset(int pix_x, int pix_y)
{
    assert(pix_x >= 0 && pix_x <= 255);
    assert(pix_y >= 0 && pix_y <= 255);
    return pix_x / 8.0f + static_cast<int>(pix_y / 8.0f) * 32;
}

size_t get_tile_data_offset(int pix_y)
{
    return (pix_y % 8) * 2;
}

uint16_t get_tile_line(rw_device &r, int pix_x, int pix_y)
{
    size_t const tile_offset{get_tile_offset(pix_x, pix_y)};

    size_t const tile_index{r.read(bg_tile_map_addr + tile_offset, device::PPU)};

    constexpr size_t tile_size{16};
    size_t tile_line_addr = get_tile_data_offset(pix_y) + (tile_index * tile_size) + tile_data_addr;
    uint8_t lo = r.read(tile_line_addr++, device::PPU);
    uint8_t hi = r.read(tile_line_addr, device::PPU);

    uint16_t res{hi};
    res <<= 8;
    res |= lo;
    return res;
}

pixel_fetcher::pixel_fetcher(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
    update_addr(m_rw_device);
}

bool pixel_fetcher::dot()
{
    m_drawing_device.before_frame();

    for (int y = 0; y < 144; ++y)
        for (int x = 0; x < 160; x += 8)
        {
            auto tile_line = get_tile_line(m_rw_device, x, y);
            auto ids = convert_line_to_ids(tile_line);
            int cc = 0;
            for (int i = x; i < x + 8; ++i)
            {
                m_drawing_device.draw_pixel(i, y, get_color_pallete_based(ids[cc]));
                ++cc;
            }
        }

    m_drawing_device.after_frame();
    return true;
}
