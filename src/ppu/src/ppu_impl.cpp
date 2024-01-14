#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>

namespace
{
constexpr uint16_t LCD_CTRL{0xFF40};
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

bool ppu::ppu_impl::dot()
{
    if (should_exit)
        return false;

    const uint8_t lcd_ctr = m_rw_device.read(LCD_CTRL, device::PPU);
    if (!checkbit(lcd_ctr, 7))
    {
        m_lcd->before_frame();
        for (int i = 0; i < 160; ++i)
            m_lcd->draw_pixel(i, 72, {1.0f, 0.f, 0.f});
        m_lcd->after_frame();
    }
    return true;
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
