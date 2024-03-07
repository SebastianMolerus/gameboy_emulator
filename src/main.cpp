#include <array>
#include <cassert>
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <ppu.hpp>
#include <lcd.hpp>
#include "mem.hpp"

namespace
{

bool quit{};
void quit_cb()
{
    quit = true;
}

void cpu_cb(registers const &reg, opcode const &op)
{
}

struct dmg : public rw_device
{
    memory mem;
    lcd m_lcd;
    cpu m_cpu;
    ppu m_ppu;

    dmg() : m_lcd{quit_cb}, m_cpu{*this, cpu_cb}, m_ppu{*this, m_lcd}
    {
    }

    uint8_t read(uint16_t addr, device d) override
    {
        if (addr == 0xFF00)
        {
            return 0xFF;
        }

        if (d == device::CPU && addr >= 0x8000 && addr <= 0x9FFF &&
            (m_ppu.current_state() == STATE::DRAWING_PIXELS))
        {
            // ignore VRAM read during Pixel Drawing
            return 0xFF;
        }

        // OAM $FE00-FE9F
        if (d == device::CPU && addr >= 0xFE00 && addr <= 0xFE9F &&
            ((m_ppu.current_state() == STATE::OAM_SCAN) ||
             (m_ppu.current_state() == STATE::DRAWING_PIXELS)))
        {
            // ignore OAM read during Pixel Drawing & OAM SCAN
            return 0xFF;
        }

        if (d == device::PPU)
            m_cpu.tick();
        return mem.read(addr, d);
    }

    void write(uint16_t addr, uint8_t data, device d) override
    {
        if (d == device::CPU && addr == 0xff40 && !checkbit(data, 7) &&
            m_ppu.current_state() != STATE::VERTICAL_BLANK)
        {
            // Cpu can disable LCD only while Vertical Blank
            return;
        }

        if (d == device::CPU && addr >= 0x8000 && addr <= 0x9FFF &&
            (m_ppu.current_state() == STATE::DRAWING_PIXELS))
        {
            // ignore CPUs VRAM write during Pixel Drawing
            return;
        }

        // OAM $FE00-FE9F
        if (d == device::CPU && addr >= 0xFE00 && addr <= 0xFE9F &&
            ((m_ppu.current_state() == STATE::OAM_SCAN) ||
             (m_ppu.current_state() == STATE::DRAWING_PIXELS)))
        {
            // ignore CPUs OAM write during Pixel Drawing & OAM SCAN
            return;
        }

        // DMA
        if (addr == 0xFF46 && d == device::CPU)
        {
            m_ppu.dma(data);
            return;
        }

        mem.write(addr, data, d);
    }

    void loop()
    {
        int cc{4};
        while (!quit)
        {
            // 1 CPU tick == 4 PPU dots
            m_ppu.dot();
            --cc;
            if (!cc)
            {
                m_cpu.tick();
                cc = 4;
            }
        }
    }
};

} // namespace

int main()
{
    dmg gameboy;
    gameboy.loop();
    return 0;
}