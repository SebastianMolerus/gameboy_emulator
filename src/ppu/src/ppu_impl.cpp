#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>
#include <array>

// FF40 — LCDC: LCD control
static constexpr uint16_t LCD_CTRL{0xFF40};
extern bool LCD_PPU_ENABLE;               // BIT7
extern uint16_t WINDOW_TILE_MAP_AREA;     // BIT6
extern bool WINDOW_ENABLE;                // BIT5
extern uint16_t BG_WINDOW_TILE_DATA_AREA; // BIT4
extern uint16_t BG_TILE_MAP;              // BIT3
extern uint8_t OBJ_SIZE;                  // BIT2
extern bool OBJ_ENABLE;                   // BIT1
extern bool BG_WINDOW_ENABLE;             // BIT0
extern void lcd_control_settings(uint8_t lcd_ctrl_value);

uint16_t BGP{}; // pallete

namespace
{
constexpr color WHITE{1.f, 1.f, 1.f};
constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
constexpr color BLACK{};

constexpr int TILE_MAP_SIZE{1024};

} // namespace

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
}

void ppu::ppu_impl::dot()
{
    const uint8_t lcd_ctrl = m_rw_device.read(LCD_CTRL, device::PPU);
    lcd_control_settings(lcd_ctrl);
    // BGP = m_rw_device.read(0xFF47, device::PPU);
    BGP = 0b11100100;

    if (!LCD_PPU_ENABLE)
    {
        return;
    }

    if (m_current_line == -1)
    {
        m_drawing_device.before_frame();
        m_current_line = 0;
    }

    switch (m_current_state)
    {
    case STATE::OAM_SCAN:
        // TODO
        ++m_current_dot;
        if (m_current_dot == 80)
            m_current_state = STATE::DRAWING_PIXELS;
        break;

    case STATE::DRAWING_PIXELS:
        draw();
        ++m_current_dot;
        if (m_current_dot == 456)
        {
            m_current_dot = 0;
            m_rw_device.write(0xFF44, m_current_line++, device::PPU);
            if (m_current_line == 144)
                m_current_state = STATE::VERTICAL_BLANK;
            else
                m_current_state = STATE::OAM_SCAN;
        }
        break;

    case STATE::HORIZONTAL_BLANK:
        // TODO
        break;
    case STATE::VERTICAL_BLANK:

        ++m_current_dot;
        if (m_current_dot == 456)
        {
            m_current_dot = 0;
            m_rw_device.write(0xFF44, m_current_line++, device::PPU);
            if (m_current_line == 154)
            {
                m_current_line = -1;
                m_current_state = STATE::OAM_SCAN;
                m_drawing_device.after_frame();
            }
        }
    }
}

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

// Read tile ( 16B ) from addr
tile read_tile(uint16_t addr, rw_device &d)
{
    tile result{};
    uint16_t new_line{};
    int idx{};
    for (int i = 0; i < 16; i += 2)
    {
        new_line = d.read(addr + i, device::PPU);
        new_line <<= 8;
        new_line |= d.read(addr + 1 + i, device::PPU);
        result.m_lines[idx++] = new_line;
    }
    return result;
}

color get_color_pallete_based(uint8_t id)
{
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

void draw_ids(int x, int y, drawing_device &d, uint8_t ids)
{
}

void draw_tile(int x, int y, drawing_device &d, tile const &t)
{
}

void ppu::ppu_impl::draw()
{
    static int offset{};
    static int cord_x{};
    static int cord_y{};

    int const index = m_rw_device.read(WINDOW_TILE_MAP_AREA + offset, device::PPU);

    if (index != 0)
    {

        tile t = read_tile((index * sizeof(tile)) + BG_WINDOW_TILE_DATA_AREA, m_rw_device);
        for (int y = 0; y < 8; ++y)
        {
            auto ids = convert_line_to_ids(t.m_lines[y]);

            for (int i = 0; i < 8; ++i)
            {
                m_drawing_device.draw_pixel(i + cord_x, y + cord_y, get_color_pallete_based(ids[i]));
            }
        }

        cord_x += 8;
        if (cord_x == 152)
        {
            cord_y += 8;
            cord_x = 0;
        }
    }

    ++offset;
    if (offset == TILE_MAP_SIZE)
    {
        offset = 0;
        cord_x = cord_y = 0;
    }
}

// ******************************************
//                  PPU PART
// ******************************************
ppu::ppu(rw_device &rw_device, drawing_device &drawing_device)
    : m_pimpl{std::make_unique<ppu::ppu_impl>(rw_device, drawing_device)}
{
}

ppu::~ppu() = default;

void ppu::dot()
{
    assert(m_pimpl);
    m_pimpl->dot();
}
