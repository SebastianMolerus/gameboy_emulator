#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <span>

TEST(LoadTest, ld_BC_n16)
{
    // 5831 == 0x16C7 -> C7, 16 in LE
    uint8_t a[] = {0x01, 0xC7, 0x16};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.BC.u16, 5831);
}

TEST(LoadTest, ld_DE_n16)
{
    // 5503 == 0x157F -> 7F, 15 in LE
    uint8_t a[] = {0x11, 0x7F, 0x15};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.DE.u16, 5503);
}

TEST(LoadTest, ld_HL_n16)
{
    // 62735 == 0xF50F -> 0F, F5 in LE
    uint8_t a[] = {0x21, 0x0F, 0xF5};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.HL.u16, 62735);
}

TEST(LoadTest, ld_SP_n16)
{
    // 43981 == 0xABCD -> CD, AB in LE
    uint8_t a[] = {0x31, 0xCD, 0xAB};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.SP.u16, 43981);
}

TEST(LoadTest, ld_HL_SP_n8)
{
    // load 62 ( 0x3E ) to SP using ld_SP_n16

    // add 35 ( 0x23 ) to SP
    // write result to HL

    uint8_t a[] = {0x31, 0x3E, 0x00, 0xF8, 0x23};
    Cpu cpu{a};

    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.HL.u16, 97);

    ASSERT_TRUE(expected_data.is_flag_set(CpuData::FLAG_H));

    ASSERT_FALSE(expected_data.is_flag_set(CpuData::FLAG_C));
    ASSERT_FALSE(expected_data.is_flag_set(CpuData::FLAG_Z));
    ASSERT_FALSE(expected_data.is_flag_set(CpuData::FLAG_N));
}