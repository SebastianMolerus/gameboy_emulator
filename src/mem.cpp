#include "mem.hpp"
#include <cassert>
#include <array>
#include <exception>
#include <iomanip>
#include <sstream>

extern std::vector<uint8_t> load_rom();
extern std::array<uint8_t, 256> load_boot_rom();

std::vector<memory::memory_area> memory::DMG_MEMORY;

memory::memory()
{
    std::array<uint8_t, 256> const boot_rom{load_boot_rom()};
    for (int i = 0; i < boot_rom.size(); ++i)
        write(i, boot_rom[i]);

    std::vector<uint8_t> const room{load_rom()};
    // to propagate cart ROM
    swap_rom();
    for (int i = 0; i < room.size(); ++i)
        write(i, room[i]);
    // switch to boot ROM
    swap_rom();
}

uint8_t memory::read(uint16_t addr, device d)
{
    for (auto &area : DMG_MEMORY)
        if (addr >= area.m_beg && addr <= area.m_end)
            return area.m_mem[addr - area.m_beg];

    std::stringstream ss;
    ss << "[READ] Cannot reach address: ";
    ss << std::hex << (int)addr << '\n';

    assert(false);
    return 0xFF;
}

void memory::write(uint16_t addr, uint8_t data, device d)
{
    for (auto &area : DMG_MEMORY)
        if (addr >= area.m_beg && addr <= area.m_end)
        {
            area.m_mem[addr - area.m_beg] = data;

            if (addr == 0xFF50 && data == 0x1)
                swap_rom();

            return;
        }

    std::stringstream ss;
    ss << "[WRITE] Cannot reach address: ";
    ss << std::hex << (int)addr << '\n';
}

void memory::swap_rom()
{
    std::swap(DMG_MEMORY[0].m_mem, DMG_MEMORY[1].m_mem);
    std::swap(DMG_MEMORY[0].m_beg, DMG_MEMORY[1].m_beg);
    std::swap(DMG_MEMORY[0].m_end, DMG_MEMORY[1].m_end);
}
