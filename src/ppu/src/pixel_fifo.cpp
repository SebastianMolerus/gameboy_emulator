#include "ppu_impl.hpp"
#include <array>
#include <cassert>
#include <deque>
#include <variant>

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
    uint8_t m_priority{};   // 0 or 1 ( BG and Window colors 1–3 are drawn over this OBJ )
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

uint16_t read_two_bytes(rw_device &rw, uint16_t addr)
{
    uint8_t const tile_lo = rw.read(addr, device::PPU, true);
    uint8_t const tile_hi = rw.read(addr + 1, device::PPU, true);
    uint16_t line = tile_hi;
    line <<= 8;
    line |= tile_lo;
    return line;
}

} // namespace

namespace
{
uint8_t current_x{};
uint8_t scroll_x{};
uint8_t scroll_y{};

uint8_t pixel_count_to_discard{};

uint8_t pushed_pixels{};

enum pixel_type
{
    BACKGROUND = 0,
    SPRITE = 1
};

using final_pixel = std::variant<bgw_pixel, sprite_pixel>;
std::deque<final_pixel> pixel_fifo;

} // namespace

void ppu::ppu_impl::update_stat(STATE s)
{
    uint8_t STAT = m_rw_device.read(0xFF41, device::PPU, true);

    // Update FF41, PPU MODE
    if (s == STATE::OAM_SCAN)
    {
        setbit(STAT, 1); // mode 2
        clearbit(STAT, 0);
    }
    else if (s == STATE::DRAWING_PIXELS)
    {
        STAT |= 0x03; // mode 3
    }
    else if (s == STATE::HORIZONTAL_BLANK)
    {
        STAT &= 0xFC; // mode 0
    }
    else
    {
        // Vertical Blank
        setbit(STAT, 0); // mode 1
        clearbit(STAT, 1);
    }

    m_rw_device.write(0xFF41, STAT, device::PPU, true);

    if ((checkbit(STAT, 3) && (s == STATE::HORIZONTAL_BLANK)) || (checkbit(STAT, 4) && (s == STATE::VERTICAL_BLANK)) ||
        (checkbit(STAT, 5) && (s == STATE::OAM_SCAN)))
    {
        STAT_INT();
    }
}

void ppu::ppu_impl::STAT_INT()
{
    uint8_t IF = m_rw_device.read(0xFF0F, device::PPU, true);
    setbit(IF, 1);
    m_rw_device.write(0xFF0F, IF, device::PPU, true);
}

bool ppu::ppu_impl::draw_pixel_line()
{
    if (current_x == 0)
    {
        scroll_x = m_rw_device.read(0xFF43, device::PPU, true);
        scroll_y = m_rw_device.read(0xFF42, device::PPU, true);
        pixel_count_to_discard = scroll_x % 8;
        m_pixel_fetcher.set_background_mode();
    }
    else
        scroll_x = m_rw_device.read(0xFF43, device::PPU, true) & 0xF8;

    screen_coordinates sc{static_cast<uint8_t>(current_x + scroll_x), static_cast<uint8_t>(m_current_line + scroll_y)};

    if (pixel_fifo.size() <= 8)
    {
        uint16_t background_line = m_pixel_fetcher.fetch_tile_line(sc).value();
        auto const colors = convert_tile_line_to_color_ids(background_line);
        for (auto c : colors)
            pixel_fifo.push_back(bgw_pixel{c});
        current_x += 8;
    }

    if (!visible_sprites.empty())
    {
        assert(pixel_fifo.size() >= 8);
        for (auto const &vs : visible_sprites)
        {
            if (vs.m_x_pos > 0 && ((vs.m_x_pos - 8) == pushed_pixels))
            {
                uint8_t const sprite_top_y = vs.m_y_pos - 16;
                uint8_t const diff = m_current_line - sprite_top_y;
                uint16_t const line_addr = vs.line_addr(diff);
                uint16_t const sprite_line = read_two_bytes(m_rw_device, line_addr);

                auto sprite_line_colors = convert_tile_line_to_color_ids(sprite_line);

                for (int i = 0; i < 8; ++i)
                {
                    if (sprite_line_colors[i] == 0) // Transparent case
                        continue;

                    switch (pixel_fifo[i].index())
                    {
                    case BACKGROUND: {
                        bgw_pixel const &p = std::get<BACKGROUND>(pixel_fifo[i]);
                        // sprite with priority 0 wins over any background pixel
                        // sprite with priority 1 wins only over background pixel with color id 0
                        // in other cases sprite pixel lose
                        if (vs.priority() == 0 || (vs.priority() == 1 && p.m_color_id == 0))
                            pixel_fifo[i] = sprite_pixel{0, sprite_line_colors[i], vs.palette()};
                        break;
                    }
                    case SPRITE:
                        // do nothing with old sprite
                        break;
                    default:
                        assert(false);
                    }
                }
            }
        }
    }

    if (pixel_fifo.size() >= 8)
    {
        if (pixel_count_to_discard)
        {
            --pixel_count_to_discard;
            pixel_fifo.pop_front();
            return false;
        }

        if (pixel_fifo.front().index() == BACKGROUND)
        {
            bgw_pixel &p = std::get<BACKGROUND>(pixel_fifo.front());
            m_drawing_device.push_pixel(p.color(m_rw_device));
        }
        else
        {
            sprite_pixel &sp = std::get<SPRITE>(pixel_fifo.front());
            m_drawing_device.push_pixel(sp.color(m_rw_device));
        }

        pixel_fifo.pop_front();
        ++pushed_pixels;

        if (pushed_pixels == 160)
        {
            pushed_pixels = current_x = scroll_x = scroll_y = pixel_count_to_discard = 0;
            std::deque<final_pixel> empty{};
            pixel_fifo.swap(empty);
            return true;
        }
    }

    return false;
}
