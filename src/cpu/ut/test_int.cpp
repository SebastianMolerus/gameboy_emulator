#include <assembler.hpp>
#include <gtest/gtest.h>
#include <cpu.hpp>
#include <array>

std::array<uint8_t, 0xFFFF + 1> memory{};

struct mem : public rw_device
{
    mem()
    {
        memory[0x00] = 0x31; // LD Sp, n16
        memory[0x01] = 0xff;
        memory[0x02] = 0xcf;
        memory[0x03] = 0xFB; // enable interrupts
        memory[0x04] = 0x0;  // NOP, Int are enabled after this instruction
        memory[0x05] = 0x3;  // INC BC
        memory[0x40] = 0xC9; // RET

        memory[0xFFFF] = 0xFF; // IE
        memory[0xFF0F] = 0x1;  // Vblank
    }
    uint8_t read(uint16_t addr, device d) override
    {
        return memory[addr];
    }
    void write(uint16_t addr, uint8_t data, device d) override
    {
        memory[addr] = data;
    }
};

void cpu_cb(registers const &reg, opcode const &op)
{
    std::cout << reg.PC() << std::endl;
}

TEST(cpu_int, init)
{
    mem m;
    cpu c{m, cpu_cb};
    for (int i = 0; i < 12; ++i)
        c.tick(); // Process LD SP, n16

    for (int i = 0; i < 8; ++i)
        c.tick(); // process EI instruction + NOP

    c.tick();

    for (int i = 0; i < 15; ++i)
        c.tick(); // process interrupt [RET]

    c.tick();
}