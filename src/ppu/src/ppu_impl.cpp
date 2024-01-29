#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>
#include <array>
#include "pixel_fetcher.hpp"

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}
{
}

void ppu::ppu_impl::dot()
{
    const uint8_t lcd_ctrl = m_rw_device.read(0xFF40, device::PPU);
    if (!checkbit(lcd_ctrl, 7))
    {
        // LCD is disabled
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
