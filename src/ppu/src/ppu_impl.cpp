#include <common.hpp>
#include "ppu_impl.hpp"
#include <array>
#include <cassert>
#include "pixel_fetcher.hpp"
#include "ppu.hpp"

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}, m_pixel_fetcher{rw_device}
{
}

void ppu::ppu_impl::dot()
{
    m_lcd_ctrl = m_rw_device.read(0xFF40, device::PPU);
    if (!checkbit(m_lcd_ctrl, 7))
    {
        m_current_dot = m_current_line = 0;
        m_current_state = STATE::OAM_SCAN;
        return;
    }

    execute_dma();

    m_rw_device.write(LCD_Y_COORDINATE, m_current_line, device::PPU);

    auto const LY_COMPARE = m_rw_device.read(0xFF45, device::PPU, true);

    if (LY_COMPARE == m_current_line)
    {
        uint8_t STAT = m_rw_device.read(0xFF41, device::PPU, true);
        setbit(STAT, 2);
        m_rw_device.write(0xFF41, STAT, device::PPU, true);

        // LYC == LY INT
        if (checkbit(STAT, 6))
        {
            uint8_t IF = m_rw_device.read(0xFF0F, device::PPU, true);
            setbit(IF, 1);
            m_rw_device.write(0xFF0F, IF, device::PPU, true);
        }
    }

    switch (m_current_state)
    {
    case STATE::OAM_SCAN:
        OAM_SCAN();
        break;
    case STATE::DRAWING_PIXELS:
        DRAWING_PIXELS();
        break;
    case STATE::HORIZONTAL_BLANK:
        HORIZONTAL_BLANK();
        break;
    case STATE::VERTICAL_BLANK:
        VERTICAL_BLANK();
        break;
    }

    ++m_current_dot;
}

void ppu::ppu_impl::dma(uint8_t src_addr)
{
    // dma_source ==  0x[src_addr]00
    dma_source = 0;
    dma_source = src_addr;
    dma_source <<= 8;
    dma_counter = 160;
}

void ppu::ppu_impl::execute_dma()
{
    if (dma_counter != 0)
    {
        uint8_t const value = m_rw_device.read(dma_source, device::PPU);
        uint16_t destination{0xFE00};
        destination |= (dma_source & 0xFF);
        m_rw_device.write(destination, value, device::PPU);
        ++dma_source;
        --dma_counter;
    }
}

STATE ppu::ppu_impl::current_state() const
{
    return m_current_state;
}

// ******************************************
//                  PPU PART
// ******************************************
ppu::ppu(rw_device &rw_device, drawing_device &drawing_device) : m_pimpl{std::make_unique<ppu::ppu_impl>(rw_device, drawing_device)}
{
}

ppu::~ppu() = default;

void ppu::dot()
{
    m_pimpl->dot();
}

void ppu::dma(uint8_t src_addr)
{
    assert(src_addr >= 0 && src_addr <= 0xDF);
    m_pimpl->dma(src_addr);
}

STATE ppu::current_state() const
{
    return m_pimpl->current_state();
}
