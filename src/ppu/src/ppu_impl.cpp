#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>
#include <array>
#include "pixel_fetcher.hpp"
#include <iostream>
#include "ppu.hpp"

ppu::ppu_impl::ppu_impl(rw_device &rw_device, drawing_device &drawing_device)
    : m_rw_device{rw_device}, m_drawing_device{drawing_device}, pf{rw_device, drawing_device}
{
}

void ppu::ppu_impl::dot()
{
    lcd_ctrl = m_rw_device.read(0xFF40, device::PPU);
    if (!checkbit(lcd_ctrl, 7))
    {
        return;
    }

    execute_dma();

    m_rw_device.write(LCD_Y_COORDINATE, m_current_line, device::PPU);

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

void ppu::dma(uint8_t src_addr)
{
    m_pimpl->dma(src_addr);
}

STATE ppu::current_state() const
{
    return m_pimpl->current_state();
}
