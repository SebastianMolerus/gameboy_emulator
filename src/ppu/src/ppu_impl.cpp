#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>

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

namespace
{
constexpr uint16_t SCY{0xFF42};
constexpr uint16_t SCX{0xFF43};

bool should_exit{};

// This is called due to user input
void on_key_callback(KEY k)
{
    if (k == KEY::ESC)
        should_exit = true;
}

} // namespace

ppu::ppu_impl::ppu_impl(rw_device &rw_device) : m_rw_device{rw_device}, m_lcd{std::make_unique<lcd>(on_key_callback)}
{
}

// return false to end work
bool ppu::ppu_impl::dot()
{
    if (should_exit)
        return false;

    const uint8_t lcd_ctrl = m_rw_device.read(LCD_CTRL, device::PPU);
    lcd_control_settings(lcd_ctrl);

    if (!LCD_PPU_ENABLE)
    {
        // LCD is turned off, draw something not related with dmg
        return true;
    }

    lcd_off_drawing();
    return true;

    if (!BG_WINDOW_ENABLE)
    {
        lcd_white();
        return true;
    }

    // Drawing

    return true;
}

void ppu::ppu_impl::lcd_off_drawing()
{
    static int pos{3};
    m_lcd->before_frame();
    m_lcd->draw_pixel((pos - 3) % 160, 72, {0, 0.4f, 0});
    m_lcd->draw_pixel((pos - 2) % 160, 72, {0, 0.6f, 0});
    m_lcd->draw_pixel((pos - 1) % 160, 72, {0, 0.8f, 0});
    m_lcd->draw_pixel(pos % 160, 72, {0, 1.0f, 0});
    m_lcd->after_frame();
    ++pos;
}

void ppu::ppu_impl::empty_frame()
{
    m_lcd->before_frame();
    m_lcd->after_frame();
}

void ppu::ppu_impl::lcd_white()
{
    m_lcd->before_frame();
    for (int x = 0; x < 160; ++x)
        for (int y = 0; y < 144; ++y)
            m_lcd->draw_pixel(x, y, {1.0f, 1.0f, 1.0f});
    m_lcd->after_frame();
}

// ******************************************
//                  PPU PART
// ******************************************
ppu::ppu(rw_device &rw_device) : m_pimpl{std::make_unique<ppu::ppu_impl>(rw_device)}
{
}

ppu::~ppu() = default;

bool ppu::dot()
{
    assert(m_pimpl);
    return m_pimpl->dot();
}
