#include "cpu.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>

TEST(CpuTest, basic_register_check)
{
    Cpu cpu;
    cpu.BC.u16 = 0x8081;
    cpu.DE.u16 = 0x8081;
    cpu.HL.u16 = 0x8081;
    cpu.PC = 0x1234;
    cpu.SP = 0x5555;
    cpu.Flag.c = true;

    EXPECT_EQ(cpu.BC.hi, 0x80);
    EXPECT_EQ(cpu.BC.lo, 0x81);
    EXPECT_EQ(cpu.DE.hi, 0x80);
    EXPECT_EQ(cpu.DE.lo, 0x81);
    EXPECT_EQ(cpu.HL.hi, 0x80);
    EXPECT_EQ(cpu.HL.lo, 0x81);
    EXPECT_EQ(cpu.PC, 0x1234);
    EXPECT_EQ(cpu.SP, 0x5555);
    EXPECT_TRUE(cpu.Flag.c);
    EXPECT_FALSE(cpu.Flag.h);
    EXPECT_FALSE(cpu.Flag.n);
    EXPECT_FALSE(cpu.Flag.z);
}

TEST(CpuTest, basic_instruction_fetch)
{
    Cpu cpu;
    std::array<uint8_t, 5> EXP_INSTRUCTIONS = {0x88, 0x89, 0x8A, 0xFA, 0x01};
    std::copy(cpu.memory.begin(), cpu.memory.end(), EXP_INSTRUCTIONS.begin());
    for (auto instruction : EXP_INSTRUCTIONS)
    {
        EXPECT_EQ(instruction, cpu.fetch_instruction(cpu.PC));
        ++cpu.PC;
    }

    EXPECT_EQ(EXP_INSTRUCTIONS.size(), cpu.PC);
}