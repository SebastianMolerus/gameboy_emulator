#include "pixel_fetcher.hpp"
#include <cassert>
#include "tile_info.hpp"

namespace
{
constexpr uint16_t scroll_y_addr{0xFF42};
constexpr uint16_t scroll_x_addr{0xFF43};
} // namespace

pixel_fetcher::pixel_fetcher(rw_device &rw_device, drawing_device &dd) : rw{rw_device}, dd{dd}
{
}

void pixel_fetcher::dot()
{
    switch (cc)
    {
    case 0: {
        tile_info ti{rw};
        tile_line_addr = ti.get_tile_line_addr(get_x(), get_y());
        ++x;

        if (x == 160)
        {
            x = 0;
            ++y;
            if (y == 144)
                y = 0;
        }
    }
    break;
    case 2:
        tile_lo = rw.read(tile_line_addr++, device::PPU);
        break;
    case 4:
        tile_hi = rw.read(tile_line_addr, device::PPU);
        break;
    case 7:
        cc = -1;
        break;
    default:
        break;
    }

    if (!q.empty())
    {
        color c = q.front();
        q.pop();
    }

    ++cc;
}

uint8_t pixel_fetcher::get_x()
{
    return rw.read(scroll_x_addr, device::PPU) + x;
}

uint8_t pixel_fetcher::get_y()
{
    return rw.read(scroll_y_addr, device::PPU) + y;
}
