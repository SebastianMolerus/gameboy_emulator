#include "mem.hpp"
#include <iostream>

extern std::vector<uint8_t> load_rom();
extern std::array<uint8_t, 256> load_boot_rom();

memory::memory()
{
    std::array<uint8_t, 256> boot{load_boot_rom()};
    for (int i = 0; i < boot.size(); ++i)
        boot_rom[i] = boot[i];

    std::vector<uint8_t> const room{load_rom()};
    for (int i = 0; i < room.size(); ++i)
        write(i, room[i]);
}

uint8_t memory::read(uint16_t addr, device d)
{
    if (boot && d == device::CPU && addr <= 0x100)
        return boot_rom[addr];
    else
        return whole_memory[addr];
}

void memory::write(uint16_t addr, uint8_t data, device d)
{
    if (addr == 0xFF50 && data == 0x1)
    {
        std::cout << "[BOOT-ROM SWAP]\n";
        boot = false;
        return;
    }

    whole_memory[addr] = data;
}
