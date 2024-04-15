#include <cpu.hpp>
#include <ppu.hpp>
#include <lcd.hpp>
#include "mem.hpp"

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

memory *ptr;

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

        // Joypad INT
        uint8_t IF = ptr->read(0xFF0F);
        setbit(IF, 4);
        ptr->write(0xFF0F, IF);
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

void cpu_callback(registers const &regs, opcode const &op)
{
}

// for debugging
registers get_start_values()
{
    registers sv;
    sv.A() = 1;
    sv.F() = 0xB0;
    sv.B() = 0;
    sv.C() = 0x13;
    sv.D() = 0;
    sv.E() = 0xD8;
    sv.H() = 1;
    sv.L() = 0x4D;
    sv.SP() = 0xFFFE;
    sv.PC() = 0x0100;
    return sv;
}

struct dmg : public rw_device
{
    memory m_mem;
    lcd m_lcd;
    cpu m_cpu;
    ppu m_ppu;

    dmg() : m_lcd{quit_cb, keyboard_cb}, m_cpu{*this, cpu_callback}, m_ppu{*this, m_lcd}
    {
        ptr = &m_mem;
    }

    uint8_t read(uint16_t addr, device d, bool direct) override
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
            case Joypad::BUTTONS: {
                if ((joypad_buttons & 0x03) < 7)
                    m_cpu.resume();
                return joypad_buttons;
            }
            case Joypad::INPUT:
                if ((joypad_buttons & 0x03) < 7)
                    m_cpu.resume();
                return joypad_input;
            }
        }

        return m_mem.read(addr, d);
    }

    void write(uint16_t addr, uint8_t data, device d, bool direct) override
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

        // Divider register, any value resets its value to 0x00
        // Normal CPU update of this registry is with flag "direct"
        if (addr == 0xFF04 && !direct)
        {
            m_mem.write(0xFF04, 0);
            return;
        }

        // DMA
        if (addr == 0xFF46 && d == device::CPU)
        {
            m_ppu.dma(data);
            return;
        }

        m_mem.write(addr, data, d);
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