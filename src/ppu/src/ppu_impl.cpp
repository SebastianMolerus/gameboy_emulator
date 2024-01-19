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

static constexpr uint16_t BGP{0xFF47}; // pallete

namespace
{
constexpr color WHITE{1.f, 1.f, 1.f};
constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
constexpr color BLACK{};

// 8x8 Pixels
struct tile
{
    std::array<uint16_t, 8> m_lines;
};

} // namespace

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
}

void ppu::ppu_impl::dot()
{
    const uint8_t lcd_ctrl = m_rw_device.read(LCD_CTRL, device::PPU);
    lcd_control_settings(lcd_ctrl);

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

tile read_tile(uint16_t offset, rw_device &d)
{
    const uint16_t tile_addr = BG_WINDOW_TILE_DATA_AREA + (offset * sizeof(tile));
    tile result{};

    uint16_t new_line{};
    for (int i = 0; i < 8; ++i)
    {
        new_line = d.read(tile_addr, device::PPU);
        new_line <<= 8;
        new_line |= d.read(tile_addr + 1 + i, device::PPU);
        result.m_lines[i] = new_line;
    }

    return result;
}

uint8_t convert_line_to_ids(uint16_t line)
{
    uint8_t const l = line >> 8; // a b c
    uint8_t const r = line;      // i j k

    // ia jb kc
    uint8_t id{};

    for (int i = 0; i < 8; i += 2)
    {
        if (checkbit(r, i))
            setbit(id, i);
        if (checkbit(l, i))
            setbit(id, i + 1);
    }
    return id;
}

void draw_ids(int x, int y, drawing_device &d, uint8_t ids)
{
}

void draw_tile(int x, int y, drawing_device &d, tile const &t)
{

    for (int j = y; j < y + 8; ++j)
        for (int i = x; i < x + 8; ++i)
        {
            uint8_t ids = convert_line_to_ids(t.m_lines[j]);
        }
}

void ppu::ppu_impl::draw()
{
    for (int offset = 0; offset < 384; ++offset)
    {
        // uint8_t const tile_idx = m_rw_device.read(WINDOW_TILE_MAP_AREA + offset, device::PPU);
        tile t = read_tile(offset, m_rw_device);
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
