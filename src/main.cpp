#include <cpu.hpp>
#include <ppu.hpp>
#include <lcd.hpp>
#include "mem.hpp"
#include <iostream>

namespace
{

enum class Joypad
{
    BUTTONS,
    INPUT,
    ALL
} joypad_mode = Joypad::ALL;

bool quit{};
void quit_cb()
{
    quit = true;
}

uint8_t joypad_buttons{0xFF};
uint8_t joypad_input{0xFF};

void keyboard_cb(key_action a, key k)
{
    if (a == key_action::down) // press
    {
        if (k == key::RIGHT)
            clearbit(joypad_input, 0);
        if (k == key::LEFT)
            clearbit(joypad_input, 1);
        if (k == key::UP)
            clearbit(joypad_input, 2);
        if (k == key::DOWN)
            clearbit(joypad_input, 3);

        if (k == key::A)
            clearbit(joypad_buttons, 0);
        if (k == key::B)
            clearbit(joypad_buttons, 1);
        if (k == key::SELECT)
            clearbit(joypad_buttons, 2);
        if (k == key::START)
            clearbit(joypad_buttons, 3);
    }
    else
    {
        if (k == key::RIGHT)
            setbit(joypad_input, 0);
        if (k == key::LEFT)
            setbit(joypad_input, 1);
        if (k == key::UP)
            setbit(joypad_input, 2);
        if (k == key::DOWN)
            setbit(joypad_input, 3);

        if (k == key::A)
            setbit(joypad_buttons, 0);
        if (k == key::B)
            setbit(joypad_buttons, 1);
        if (k == key::SELECT)
            setbit(joypad_buttons, 2);
        if (k == key::START)
            setbit(joypad_buttons, 3);
    }
}

void cpu_callback(registers const &, opcode const &op)
{
}

struct dmg : public rw_device
{
    memory mem;
    lcd m_lcd;
    cpu m_cpu;
    ppu m_ppu;

    dmg() : m_lcd{quit_cb, keyboard_cb}, m_cpu{*this, cpu_callback}, m_ppu{*this, m_lcd}
    {
    }

    uint8_t read(uint16_t addr, device d) override
    {
        if (d == device::PPU)
            m_cpu.tick();

        if (addr == 0xFF00)
        {
            // 1 - is not pressed
            // 0 - is pressed

            switch (joypad_mode)
            {
            case Joypad::ALL:
                return 0xFF;
            case Joypad::BUTTONS:
                return joypad_buttons;
            case Joypad::INPUT:
                return joypad_input;
            }
        }

        if (d == device::CPU && addr >= 0x8000 && addr <= 0x9FFF && (m_ppu.current_state() == STATE::DRAWING_PIXELS))
        {
            // ignore VRAM read during Pixel Drawing
            return 0xFF;
        }

        // OAM $FE00-FE9F
        if (d == device::CPU && addr >= 0xFE00 && addr <= 0xFE9F &&
            ((m_ppu.current_state() == STATE::OAM_SCAN) || (m_ppu.current_state() == STATE::DRAWING_PIXELS)))
        {
            // ignore OAM read during Pixel Drawing & OAM SCAN
            return 0xFF;
        }

        return mem.read(addr, d);
    }

    void write(uint16_t addr, uint8_t data, device d) override
    {
        // Joypad
        if (addr == 0xFF00)
        {
            if (!checkbit(data, 5))
                joypad_mode = Joypad::BUTTONS;
            else if (!checkbit(data, 4))
                joypad_mode = Joypad::INPUT;
            else if (checkbit(data, 5) && checkbit(data, 4))
                joypad_mode = Joypad::ALL;
        }

        if (d == device::CPU && addr == 0xff40 && !checkbit(data, 7) && m_ppu.current_state() != STATE::VERTICAL_BLANK)
        {
            // Cpu can disable LCD only while Vertical Blank
            return;
        }

        if (d == device::CPU && addr >= 0x8000 && addr <= 0x9FFF && (m_ppu.current_state() == STATE::DRAWING_PIXELS))
        {
            // ignore CPUs VRAM write during Pixel Drawing
            return;
        }

        // OAM $FE00-FE9F
        if (d == device::CPU && addr >= 0xFE00 && addr <= 0xFE9F &&
            ((m_ppu.current_state() == STATE::OAM_SCAN) || (m_ppu.current_state() == STATE::DRAWING_PIXELS)))
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