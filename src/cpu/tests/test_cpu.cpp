#include "cpu.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>

TEST(CpuTest, basic_register_check)
{
    Cpu cpu;
    cpu.registers.BC = 0x8081;
    cpu.registers.DE = 0x8081;
    cpu.registers.HL = 0x8081;
    cpu.registers.PC = 0x1234;
    cpu.registers.SP = 0x5555;
    cpu.registers.F.c = true;

    EXPECT_EQ(*cpu.registers.B, 0x80);
    EXPECT_EQ(*cpu.registers.C, 0x81);
    EXPECT_EQ(*cpu.registers.D, 0x80);
    EXPECT_EQ(*cpu.registers.E, 0x81);
    EXPECT_EQ(*cpu.registers.H, 0x80);
    EXPECT_EQ(*cpu.registers.L, 0x81);
    EXPECT_EQ(cpu.registers.PC, 0x1234);
    EXPECT_EQ(cpu.registers.SP, 0x5555);
    EXPECT_TRUE(cpu.registers.F.c);
    EXPECT_FALSE(cpu.registers.F.h);
    EXPECT_FALSE(cpu.registers.F.n);
    EXPECT_FALSE(cpu.registers.F.z);
}

TEST(CpuTest, basic_instruction_fetch)
{
    Cpu cpu;
    std::array<uint8_t, 5> EXP_INSTRUCTIONS = {0x88, 0x89, 0x8A, 0xFA, 0x01};
    std::copy(cpu.memory.begin(), cpu.memory.end(), EXP_INSTRUCTIONS.begin());
    for (auto instruction : EXP_INSTRUCTIONS)
    {
        EXPECT_EQ(instruction, cpu.fetch_instruction(cpu.registers.PC));
        ++cpu.registers.PC;
    }

    EXPECT_EQ(EXP_INSTRUCTIONS.size(), cpu.registers.PC);
}