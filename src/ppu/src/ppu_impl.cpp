#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>

namespace
{
constexpr uint16_t LCD_CTRL{0xFF40};
constexpr uint16_t SCY{0xFF42};
constexpr uint16_t SCX{0xFF43};

// This is called due to user input
void on_key_callback(KEY k)
{
}

} // namespace

ppu::ppu_impl::ppu_impl(rw_device &rw_device) : m_rw_device{rw_device}, m_lcd{std::make_unique<lcd>(on_key_callback)}
{
}

bool ppu::ppu_impl::dot()
{
    m_lcd->before_frame();

    m_lcd->draw_pixel(50, 50, {1.0f, 0.f, 0.f});

    m_lcd->after_frame();
    return false;
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
