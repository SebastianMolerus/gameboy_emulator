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

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_lcd{drawing_device}
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

    if (m_current_line == -1)
    {
        m_lcd.before_frame();
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
    case STATE::HORIZONTAL_BLANK:
        // TODO Drawing
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
                m_lcd.after_frame();
            }
        }
    }

    return true;
}

// ******************************************
//                  PPU PART
// ******************************************
ppu::ppu(rw_device &rw_device, drawing_device &drawing_device)
    : m_pimpl{std::make_unique<ppu::ppu_impl>(rw_device, drawing_device)}
{
}

ppu::~ppu() = default;

bool ppu::dot()
{
    assert(m_pimpl);
    return m_pimpl->dot();
}
