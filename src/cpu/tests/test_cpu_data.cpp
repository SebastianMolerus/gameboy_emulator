#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>

TEST(CpuDataTest, get_data_by_register_name)
{
    CpuData cpu_data;
    cpu_data.AF.u16 = 0x8081;
    cpu_data.BC.u16 = 0x8283;
    cpu_data.DE.u16 = 0x8485;
    cpu_data.HL.u16 = 0x8687;

    EXPECT_EQ(0x8081, *cpu_data.get_word("AF"));
    EXPECT_EQ(0x80, *cpu_data.get_byte("A"));

    EXPECT_EQ(0x8283, *cpu_data.get_word("BC"));
    EXPECT_EQ(0x82, *cpu_data.get_byte("B"));
    EXPECT_EQ(0x83, *cpu_data.get_byte("C"));

    EXPECT_EQ(0x8485, *cpu_data.get_word("DE"));
    EXPECT_EQ(0x84, *cpu_data.get_byte("D"));
    EXPECT_EQ(0x85, *cpu_data.get_byte("E"));

    EXPECT_EQ(0x8687, *cpu_data.get_word("HL"));
    EXPECT_EQ(0x86, *cpu_data.get_byte("H"));
    EXPECT_EQ(0x87, *cpu_data.get_byte("L"));
}