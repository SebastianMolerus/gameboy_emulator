#include <cpu.hpp>
#include <ppu.hpp>
#include <lcd.hpp>
#include "mem.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

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

std::ofstream ofs{"dr_logs.txt"};

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

void add_to_log(const char *reg_name, uint8_t reg_value)
{
    ofs << reg_name << std::setw(2) << std::setfill('0') << std::hex << (int)reg_value << " ";
}

void cpu_callback(registers const &regs, opcode const &op)
{

    // add_to_log("A:", regs.m_AF.m_hi);
    // add_to_log("F:", regs.m_AF.m_lo);
    // add_to_log("B:", regs.m_BC.m_hi);
    // add_to_log("C:", regs.m_BC.m_lo);
    // add_to_log("D:", regs.m_DE.m_hi);
    // add_to_log("E:", regs.m_DE.m_lo);
    // add_to_log("H:", regs.m_HL.m_hi);
    // add_to_log("L:", regs.m_HL.m_lo);
    // ofs << "SP:" << std::setw(4) << std::setfill('0') << std::hex << (int)regs.m_SP.m_u16 << " ";
    // ofs << "PC:" << std::setw(4) << std::setfill('0') << std::hex << (int)regs.m_PC.m_u16 << " ";

    // auto pc = regs.m_PC.m_u16;

    // auto v1 = ptr->whole_memory[pc++];
    // auto v2 = ptr->whole_memory[pc++];
    // auto v3 = ptr->whole_memory[pc++];
    // auto v4 = ptr->whole_memory[pc++];

    // ofs << "PCMEM:" << std::setw(2) << std::setfill('0') << std::hex << (int)v1 << ",";
    // ofs << std::setw(2) << std::setfill('0') << std::hex << (int)v2 << ",";
    // ofs << std::setw(2) << std::setfill('0') << std::hex << (int)v3 << ",";
    // ofs << std::setw(2) << std::setfill('0') << std::hex << (int)v4;

    // ofs << "\n";
}

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
    memory mem;
    lcd m_lcd;
    cpu m_cpu;
    ppu m_ppu;

    dmg() : m_lcd{quit_cb, keyboard_cb}, m_cpu{*this, cpu_callback}, m_ppu{*this, m_lcd}
    {
        // ofs << "A:01 F:b0 B:00 C:13 D:00 E:d8 H:01 L:4d SP:fffe PC:0100 PCMEM:00,c3,13,02" << '\n';
        ptr = &mem;
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

        return mem.read(addr, d);
    }

    void write(uint16_t addr, uint8_t data, device d, bool direct) override
    {
        if (addr == 0xFFFF && !direct && !checkbit(data, 0))
        {
            std::cout << "IE VBLANK DISABLED\n";
        }

        if (addr == 0xFFFF && checkbit(data, 0))
        {
            std::cout << "IE VBLANK ENABLED\n";
        }
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
            std::cout << "Divider reset\n";
            mem.write(0xFF04, 0);
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