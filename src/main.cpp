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
#include "log.hpp"
#include <windows.h>

namespace
{

struct dmg;
dmg *dmg_ptr{nullptr};

registers curr_regs;
opcode curr_op;

bool quit{};
void quit_cb()
{
    quit = true;
}

bool step{};
void step_button();

void continue_button()
{
    step = false;
}

std::stringstream debug_ss(registers const &reg, opcode const &op)
{
    static int curr_line{};
    std::stringstream ss;
    ss << "0x" << std::setw(4) << std::setfill('0') << std::hex << curr_line << ": "
       << op.m_mnemonic << " ";
    for (auto i = 0; i < op.operands_size(); ++i)

    {
        auto const &oper = op.m_operands[i];
        auto const oper_name = oper.m_name;
        if (!std::strcmp(oper_name, "n8") || !std::strcmp(oper_name, "a8") ||
            !std::strcmp(oper_name, "e8"))
            ss << "0x" << std::setw(3) << std::setfill('0') << std::hex
               << static_cast<int>(op.m_data[0]);
        else if (!std::strcmp(oper_name, "n16") || !std::strcmp(oper_name, "a16"))
            ss << "0x" << std::setw(3) << std::setfill('0') << std::hex
               << static_cast<int>(op.m_data[1]) << static_cast<int>(op.m_data[0]);
        else
            ss << oper_name;
        if (!i && op.operands_size() > 1)
            ss << ", ";
    }
    ss << " \n";
    curr_line = reg.get_pc();
    return ss;
}

void cpu_cb(registers const &reg, opcode const &op)
{
}

constexpr uint16_t pc_value{0x1D};
constexpr uint16_t sp_value{0xFFFE};
const registers startup_value{pc_value, sp_value};

struct dmg : public rw_device
{
    memory mem;
    lcd m_lcd;
    cpu m_cpu;
    ppu m_ppu;
    int cc{4};

    dmg()
        : m_lcd{quit_cb, step_button, continue_button}, m_cpu{*this, cpu_cb, startup_value},
          m_ppu{*this, m_lcd}
    {
    }

    uint8_t read(uint16_t addr, device d) override
    {
        if (addr == 0xFF41)
        {
            int a = 10;
        }

        if (d == device::PPU)
            m_cpu.tick();
        return mem.read(addr, d);
    }

    void write(uint16_t addr, uint8_t data, device d) override
    {
        if (addr == 0xFF40)
        {
            std::cout << "Writing to FF40: " << std::hex << (int)data << std::endl;
        }

        if (addr == 0xFF07)
        {
            std::cout << "TIMER\n";
        }

        // DMA
        if (addr == 0xFF46)
        {

            m_ppu.dma(data);
            return;
        }

        if (addr == 0xFF50 && data == 0x1)
            ::step = true;

        mem.write(addr, data, d);
    }

    void step()
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
};

void step_button()
{
    // registers saved = curr_regs;
    // while (saved.PC() == curr_regs.PC())
    //     dmg_ptr->step();

    step = true;
    // while (step)
    // {
    //     MSG msg{};
    //     if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    //     {
    //         if (msg.message == WM_QUIT)
    //         {
    //             quit = true;
    //             return;
    //         }
    //         ::TranslateMessage(&msg);
    //         ::DispatchMessage(&msg);
    //     }
    // }
}

} // namespace

int main()
{
    dmg gameboy;
    dmg_ptr = &gameboy;
    while (!quit)
        gameboy.step();
    return 0;
}