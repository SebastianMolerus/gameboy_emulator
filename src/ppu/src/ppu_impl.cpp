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

constexpr color WHITE{1.f, 1.f, 1.f};
constexpr color LIGHT_GRAY{0.867f, 0.706f, 0.71f};
constexpr color DARK_GRAY{0.38f, 0.31f, 0.302f};
constexpr color BLACK{};

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
        return true;
    }

    if (!BG_WINDOW_ENABLE)
    {
        lcd_white();
        return true;
    }

    if (m_current_line == -1)
    {
        m_lcd->before_frame();
        m_current_line = 0;
    }

    if (m_current_dot < 80) // OAM SCAN
    {
        // EMPTY for NOW
        ++m_current_dot;
    }
    else if (m_current_dot >= 80 && m_current_dot < 456)
    {
        if (((m_current_dot - 80) < 160) && m_current_line < 144)
            m_lcd->draw_pixel(m_current_dot - 80, m_current_line, DARK_GRAY);
        ++m_current_dot;
    }
    else
    {
        // Whole pixel line is ready, increment line
        m_rw_device.write(0xFF44, m_current_line++, device::PPU);
        m_current_dot = 0;

        // whole screen was drawn
        if (m_current_line == 154)
        {
            m_lcd->after_frame();
            m_current_line = -1;
        }
    }

    return true;
}

void ppu::ppu_impl::lcd_off_drawing()
{
    m_lcd->before_frame();
    for (int i = 0; i < 160; ++i)
        m_lcd->draw_pixel(i, 72, {0.7f, 0, 0});

    m_lcd->after_frame();
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
