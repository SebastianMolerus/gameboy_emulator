#include "pixel_fetcher.hpp"
#include <cassert>
#include "tile_info.hpp"

namespace
{
constexpr uint16_t scroll_y_addr{0xFF42};
constexpr uint16_t scroll_x_addr{0xFF43};

rw_device *RW;

color get_color_pallete_based(uint8_t id)
{
    constexpr color WHITE{1.f, 1.f, 1.f};
    constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
    constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
    constexpr color BLACK{};

    uint8_t const bg_palette = RW->read(0xFF47, device::PPU);

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

std::array<color, 8> convert_tile_line_to_colors(uint16_t line)
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

} // namespace

pixel_fetcher::pixel_fetcher(rw_device &rw_device, drawing_device &dd) : rw{rw_device}, dd{dd}
{
    RW = &(this->rw);
}

pixel_fetcher::LINE_DRAWING_STATUS pixel_fetcher::dot(uint8_t screen_line)
{
    switch (dot_counter)
    {
    case 0: {
        auto const y_scroll = rw.read(scroll_y_addr, device::PPU);
        auto const x_scroll = rw.read(scroll_x_addr, device::PPU);
        tile_info ti{rw};
        auto tile_line_addr = ti.get_tile_line_addr(x_scroll + x, screen_line + y_scroll);
        uint8_t const tile_lo = rw.read(tile_line_addr++, device::PPU);
        uint8_t const tile_hi = rw.read(tile_line_addr, device::PPU);

        uint16_t line = tile_hi;
        line <<= 8;
        line |= tile_lo;

        pixels = convert_tile_line_to_colors(line);
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
            x += 8;
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
        x = dot_counter = 0;
        std::queue<color> empty;
        swap(pixel_fifo, empty);
        pushed_px = 0;
        return LINE_DRAWING_STATUS::DONE;
    }
    else
        return LINE_DRAWING_STATUS::IN_PROGRESS;
}
