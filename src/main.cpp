#include <array>
#include <cassert>
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

extern std::vector<uint8_t> load_rom();
extern std::array<uint8_t, 256> load_boot_rom();
extern void print_op(registers const &reg, opcode const &op);

namespace
{

void cpu_cb(registers const &reg, opcode const &op)
{
    print_op(reg, op);
}

struct memory_area
{
    std::vector<uint8_t> m_mem;
    uint16_t m_beg;
    uint16_t m_end;
};

std::vector<memory_area> DMG_MEMORY;

// swap boot_rom <-> rom
void swap_rom()
{
    std::swap(DMG_MEMORY[0].m_mem, DMG_MEMORY[1].m_mem);
    std::swap(DMG_MEMORY[0].m_beg, DMG_MEMORY[1].m_beg);
    std::swap(DMG_MEMORY[0].m_end, DMG_MEMORY[1].m_end);
}

uint8_t dmg_memory_read(uint16_t addr)
{
    for (auto &area : DMG_MEMORY)
        if (addr >= area.m_beg && addr <= area.m_end)
            return area.m_mem[addr - area.m_beg];

    static uint8_t def{0xFF};
    return def;
}

void dmg_memory_write(uint16_t addr, uint8_t data)
{
    for (auto &area : DMG_MEMORY)
        if (addr >= area.m_beg && addr <= area.m_end)
        {
            area.m_mem[addr - area.m_beg] = data;

            if (addr == 0xFF50 && data == 0x1)
                swap_rom();

            return;
        }
}

template <uint16_t beg, uint16_t end = beg> struct memory_block
{
    memory_block()
    {
        memory_area new_area;
        new_area.m_mem = std::vector<uint8_t>(end - beg + 1, {});
        new_area.m_beg = beg;
        new_area.m_end = end;
        DMG_MEMORY.push_back(std::move(new_area));
    }
};

struct dmg : public rw_device
{
    memory_block<0, 0xFF> m_BOOT_ROM; // swappable with m_ROM
    memory_block<0, 0x7FFF> m_ROM;    // swappable with m_BOOT_ROM

    memory_block<0x8000, 0x9fff> m_VRAM;
    memory_block<0xFF11> m_CH1_T_D;           // Channel 1 length timer & duty cycle
    memory_block<0xFF12> m_CH1_V_E;           // Channel 1 volume & envelope
    memory_block<0xFF24> m_MASTER_VOLUME;     // Master volume & VIN panning
    memory_block<0xFF25> m_SOUND_PANNING;     // Sound panning
    memory_block<0xFF26> m_AUDIO_MASTER_CTRL; // Audio master control
    memory_block<0xFF44> m_LY;                // LCD Y coordinate
    memory_block<0xFF47> m_BGP;               // BG palette data
    memory_block<0xFF50> m_BOOT_ROM_DISABLE;  // 0x1 hex here and boot rom is disabled

    cpu m_cpu;

    dmg() : m_cpu{*this, cpu_cb}
    {
        std::array<uint8_t, 256> const boot_rom{load_boot_rom()};
        for (int i = 0; i < boot_rom.size(); ++i)
            dmg_memory_write(i, boot_rom[i]);

        std::vector<uint8_t> const room{load_rom()};
        // to propagate cart ROM
        swap_rom();
        for (int i = 0; i < room.size(); ++i)
            dmg_memory_write(i, room[i]);
        // switch to boot ROM
        swap_rom();
    }

    uint8_t read(uint16_t addr) override
    {
        return dmg_memory_read(addr);
    }

    void write(uint16_t addr, uint8_t data) override
    {
        dmg_memory_write(addr, data);
    }

    void start()
    {
        while (1)
        {
            m_cpu.tick();
        }
    }
};

} // namespace

int main()
{
    dmg gameboy;
    gameboy.start();
    return 0;
}