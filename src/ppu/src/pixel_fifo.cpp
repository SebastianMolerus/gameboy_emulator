#include "ppu_impl.hpp"
#include <array>
#include <cassert>

namespace
{

constexpr color WHITE{1.f, 1.f, 1.f};
constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
constexpr color BLACK{};

std::array<color, 4> VALUE_COLOR_MAP{WHITE, LIGHT_GRAY, DARK_GRAY, BLACK};

color get_color(uint8_t color_id, uint8_t palette)
{
    switch (color_id)
    {
    case 0:
        return VALUE_COLOR_MAP[palette & 0x03];
    case 1:
        return VALUE_COLOR_MAP[palette >> 2 & 0x03];
    case 2:
        return VALUE_COLOR_MAP[palette >> 4 & 0x03];
    case 3:
        return VALUE_COLOR_MAP[palette >> 6 & 0x03];
    default:
        assert(false);
    }
}

struct sprite_pixel
{
    uint8_t m_priority{};   // 0 or 1
    uint8_t m_color_id{};   // 0-3, 0 means transparent
    uint8_t m_palette_id{}; // 0 or 1

    color color(rw_device &rw) const
    {
        assert(m_color_id != 0);
        uint8_t palette{};
        if (m_palette_id == 0)
            palette = rw.read(0xFF48, device::PPU);
        else
            palette = rw.read(0xFF49, device::PPU);
        return get_color(m_color_id, palette);
    }
};

struct bgw_pixel
{
    uint8_t m_color_id{}; // 0-3

    color color(rw_device &rw) const
    {
        uint8_t val{};
        uint8_t const palette = rw.read(0xFF47, device::PPU);
        return get_color(m_color_id, palette);
    }
};

std::array<uint8_t, 8> convert_tile_line_to_color_ids(uint16_t line)
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

} // namespace

namespace LOL
{
uint8_t current_x{};
uint8_t scroll_x{};
uint8_t scroll_y{};

uint8_t pixel_count_to_discard{};

uint8_t pushed_pixels{};
} // namespace LOL

using namespace LOL;

bool ppu::ppu_impl::draw_pixel_line()
{
    if (current_x == 0)
    {
        scroll_x = m_rw_device.read(0xFF43, device::PPU);
        scroll_y = m_rw_device.read(0xFF42, device::PPU);
        pixel_count_to_discard = scroll_x % 8;
    }
    else
        scroll_x = m_rw_device.read(0xFF43, device::PPU) & 0xF8;

    screen_coordinates sc{static_cast<uint8_t>(current_x + scroll_x), static_cast<uint8_t>(m_current_line + scroll_y)};

    if (auto pixels = m_pixel_fetcher.fetch_tile_line(sc); pixels)
    {
        std::array<uint8_t, 8> ids = convert_tile_line_to_color_ids(pixels.value());

        for (auto id : ids)
        {
            if (pixel_count_to_discard)
                --pixel_count_to_discard;
            else
            {
                m_drawing_device.push_pixel(bgw_pixel{id}.color(m_rw_device));
                ++pushed_pixels;

                // End of the line
                if (pushed_pixels == 160)
                {
                    pushed_pixels = current_x = scroll_x = scroll_y = pixel_count_to_discard = 0;
                    return true;
                }
            }
        }

        current_x += 8;
    }

    return false;
}