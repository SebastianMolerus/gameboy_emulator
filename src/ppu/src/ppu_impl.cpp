#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>
#include <array>
#include "pixel_fetcher.hpp"

// FF40 — LCDC: LCD control
static constexpr uint16_t LCD_CTRL_ADDR{0xFF40};
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

constexpr int TILE_MAP_SIZE{1024};

} // namespace

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
}

void ppu::ppu_impl::dot()
{
    const uint8_t lcd_ctrl = m_rw_device.read(LCD_CTRL_ADDR, device::PPU);
    lcd_control_settings(lcd_ctrl);
    // BGP = m_rw_device.read(0xFF47, device::PPU);
    BGP = 0b11100100;

    if (!LCD_PPU_ENABLE)
    {
        return;
    }

    if (m_current_line == -1)
    {
        // m_drawing_device.before_frame();
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
                // m_drawing_device.after_frame();
            }
        }
    }
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

void ppu::ppu_impl::draw()
{
    pixel_fetcher pf{m_rw_device, m_drawing_device};

    pf.dot();
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
