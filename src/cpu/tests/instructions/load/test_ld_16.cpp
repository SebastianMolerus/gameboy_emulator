#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <translator.hpp>

TEST(test_load_16bit, ld_BC_n16)
{
    std::string assembly{R"(
        LD BC, 0x16C7
        LD BC, 0x8
        LD BC, 0x76
        LD BC, 0x5E9
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].BC.u16, 0x16C7);
    ASSERT_EQ(expected_data[0].BC.lo, 0xC7);
    ASSERT_EQ(expected_data[0].BC.hi, 0x16);

    ASSERT_EQ(expected_data[1].BC.u16, 0x8);
    ASSERT_EQ(expected_data[1].BC.lo, 0x8);
    ASSERT_EQ(expected_data[1].BC.hi, 0x0);

    ASSERT_EQ(expected_data[2].BC.u16, 0x76);
    ASSERT_EQ(expected_data[2].BC.lo, 0x76);
    ASSERT_EQ(expected_data[2].BC.hi, 0x0);

    ASSERT_EQ(expected_data[3].BC.u16, 0x5E9);
    ASSERT_EQ(expected_data[3].BC.lo, 0xE9);
    ASSERT_EQ(expected_data[3].BC.hi, 0x5);
}

TEST(test_load_16bit, ld_DE_n16)
{
    std::string assembly{R"(
        LD DE, 0x157F
        LD DE, 0x0
        LD DE, 0x12
        LD DE, 0x87F
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].DE.u16, 0x157F);
    ASSERT_EQ(expected_data[0].DE.lo, 0x7F);
    ASSERT_EQ(expected_data[0].DE.hi, 0x15);

    ASSERT_EQ(expected_data[1].DE.u16, 0x0);
    ASSERT_EQ(expected_data[1].DE.lo, 0x0);
    ASSERT_EQ(expected_data[1].DE.hi, 0x0);

    ASSERT_EQ(expected_data[2].DE.u16, 0x12);
    ASSERT_EQ(expected_data[2].DE.lo, 0x12);
    ASSERT_EQ(expected_data[2].DE.hi, 0x0);

    ASSERT_EQ(expected_data[3].DE.u16, 0x87F);
    ASSERT_EQ(expected_data[3].DE.lo, 0x7F);
    ASSERT_EQ(expected_data[3].DE.hi, 0x8);
}

TEST(test_load_16bit, ld_HL_n16)
{
    std::string assembly{R"(
        LD HL, 0xF50F
        LD HL, 0xA
        LD HL, 0xBA
        LD HL, 0xCBA
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].HL.u16, 0xF50F);
    ASSERT_EQ(expected_data[0].HL.lo, 0xF);
    ASSERT_EQ(expected_data[0].HL.hi, 0xF5);

    ASSERT_EQ(expected_data[1].HL.u16, 0xA);
    ASSERT_EQ(expected_data[1].HL.lo, 0xA);
    ASSERT_EQ(expected_data[1].HL.hi, 0x0);

    ASSERT_EQ(expected_data[2].HL.u16, 0xBA);
    ASSERT_EQ(expected_data[2].HL.lo, 0xBA);
    ASSERT_EQ(expected_data[2].HL.hi, 0x0);

    ASSERT_EQ(expected_data[3].HL.u16, 0xCBA);
    ASSERT_EQ(expected_data[3].HL.lo, 0xBA);
    ASSERT_EQ(expected_data[3].HL.hi, 0xC);
}

TEST(test_load_16bit, ld_SP_n16)
{
    std::string assembly{R"(
        LD SP, 0x7800
        LD SP, 0xF
        LD SP, 0x2A
        LD SP, 0x87B
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].SP.u16, 0x7800);
    ASSERT_EQ(expected_data[0].SP.lo, 0x00);
    ASSERT_EQ(expected_data[0].SP.hi, 0x78);

    ASSERT_EQ(expected_data[1].SP.u16, 0xF);
    ASSERT_EQ(expected_data[1].SP.lo, 0xF);
    ASSERT_EQ(expected_data[1].SP.hi, 0x0);

    ASSERT_EQ(expected_data[2].SP.u16, 0x2A);
    ASSERT_EQ(expected_data[2].SP.lo, 0x2A);
    ASSERT_EQ(expected_data[2].SP.hi, 0x0);

    ASSERT_EQ(expected_data[3].SP.u16, 0x87B);
    ASSERT_EQ(expected_data[3].SP.lo, 0x7B);
    ASSERT_EQ(expected_data[3].SP.hi, 0x8);
}