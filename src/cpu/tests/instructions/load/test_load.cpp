#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <span>
#include <vector>

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
    // 1.
    // load 62 ( 0x3E ) to SP using ld_SP_n16
    // add 35 ( 0x23 ) to SP
    // HC expected

    // 2.
    // load 0xFFFF to SP
    // add 1 to SP
    // CF expected

    // 3.
    // load 0x05 to SP
    // substract 1 from SP ( 0x81 )

    uint8_t a[] = {0x31, 0x3E, 0x00, 0xF8, 0x23, 0x31, 0xFF, 0xFF, 0xF8, 0x01, 0x31, 0x05, 0x00, 0xF8, 0x81};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[2].PC.u16, sizeof(a) / sizeof(uint8_t));

    for (auto &data : expected_data)
    {
        ASSERT_FALSE(data.is_flag_set(CpuData::FLAG_Z));
        ASSERT_FALSE(data.is_flag_set(CpuData::FLAG_N));
    }

    ASSERT_EQ(expected_data[0].HL.u16, 97);
    ASSERT_TRUE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));

    ASSERT_EQ(expected_data[1].HL.u16, 0);
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));

    ASSERT_EQ(expected_data[2].HL.u16, 4);
}

TEST(LoadTest, ld_HL_SP_n8_half_carry)
{
    // 1.
    // load 0x09 to SP
    // add 0x10 ( 16 ) to SP

    // 0000 1001
    //+0001 0000
    // No HC expected

    // 2.
    // load 0xA to SP
    // add 0xC to SP

    // 0000 1010
    //+0000 1100
    // HC expected

    uint8_t a[] = {0x31, 0x09, 0x00, 0xF8, 0x10, 0x31, 0x0A, 0x00, 0xF8, 0x0C};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
}

TEST(LoadTest, ld_HL_SP_n8_carry)
{
    // 1.
    // load 0xFFFE to SP
    // add 0x01 to SP
    // No C expected

    // 2.
    // load 0xFFFF to SP
    // add 0x01 to SP
    // C expected

    uint8_t a[] = {0x31, 0xFE, 0xFF, 0xF8, 0x01, 0x31, 0xFF, 0xFF, 0xF8, 0x01};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));
}

TEST(LoadTest, ld_SP_a16)
{
    // load 0xCDAB to SP

    uint8_t a[] = {0x08, 0xAB, 0xCD};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0xCDAB);
}
