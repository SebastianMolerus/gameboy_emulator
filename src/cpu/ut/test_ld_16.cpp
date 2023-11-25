#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <translator.hpp>

uint16_t read_word_from_stack(CpuData const &data)
{
    uint16_t result = data.m_memory[data.SP.u16 + 1];
    result <<= 8;
    result |= data.m_memory[data.SP.u16];
    return result;
}

// 0x01
TEST(test_load_16bit, LD_BC_n16)
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
    cpu.after_exec_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
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

// 0x11
TEST(test_load_16bit, LD_DE_n16)
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
    cpu.after_exec_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
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

// 0x21
TEST(test_load_16bit, LD_HL_n16)
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
    cpu.after_exec_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
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

// 0x31
TEST(test_load_16bit, LD_SP_n16)
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
    cpu.after_exec_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data.push_back(d); });
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

// 0xF8
TEST(test_load_16bit, LD_HL_SP_e8)
{
    std::string assembly{R"(
        LD SP, 0x3E
        LD HL, SP + 0x23
        LD SP, 0xFFFF
        LD HL, SP + 0x1
        LD SP, 0x5
        LD HL, SP + 0x81
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == get_opcode("LD HL, SP + e8").hex)
            expected_data.push_back(d);
    };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);

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

// 0xF8
TEST(test_load_16bit, LD_HL_SP_e8_half_carry)
{
    std::string assembly{R"(
        LD SP, 0x9
        LD HL, SP + 0x10
        LD SP, 0xA
        LD HL, SP + 0xC
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == get_opcode("LD HL, SP + e8").hex)
            expected_data.push_back(d);
    };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 2);

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
}

// 0xF8
TEST(test_load_16bit, LD_HL_SP_e8_carry)
{
    std::string assembly{R"(
        LD SP, 0xFFFE
        LD HL, SP + 0x1
        LD SP, 0xFFFF
        LD HL, SP + 0x1
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == get_opcode("LD HL, SP + e8").hex)
            expected_data.push_back(d);
    };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 2);

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));
}

// 0x08
TEST(test_load_16bit, LD_Ia16I_SP)
{
    std::string assembly{R"(
        LD SP, 0xBBAA
        LD [0x1], SP
        LD [0xFFFF], SP
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x1], 0xAA);
    ASSERT_EQ(expected_data.m_memory[0x2], 0xBB);
}

// 0xF9
TEST(test_load_16bit, LD_SP_HL)
{
    std::string assembly{R"(
        LD HL, 0xABCD
        LD SP, HL
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.HL.u16, 0xABCD);
    ASSERT_EQ(expected_data.SP.u16, expected_data.HL.u16);
}

// 0xC5
TEST(test_load_16bit, PUSH_BC)
{
    std::string assembly{R"(
        LD SP, 0xA
        LD BC, 0x548A
        PUSH BC
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 8);
    ASSERT_EQ(read_word_from_stack(expected_data), 0x548A);
}

// 0xD5
TEST(test_load_16bit, PUSH_DE)
{
    std::string assembly{R"(
        LD SP, 0xBB
        LD DE, 0xAAFF
        PUSH DE
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xBB - 2);
    ASSERT_EQ(read_word_from_stack(expected_data), 0xAAFF);
}

// 0xE5
TEST(test_load_16bit, PUSH_HL)
{
    std::string assembly{R"(
        LD SP, 0xAB
        LD HL, 0x1234
        PUSH HL
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xAB - 2);
    ASSERT_EQ(read_word_from_stack(expected_data), 0x1234);
}

// 0xF5
TEST(test_load_16bit, PUSH_AF)
{
    std::string assembly{R"(
        LD SP, 0x100
        LD DE, 0xBD00
        LD A, D
        PUSH AF
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0x0100 - 2);
    ASSERT_EQ(read_word_from_stack(expected_data), 0xBD00);
}

// 0xC1
TEST(test_load_16bit, pop_BC)
{
    std::string assembly{R"(
        LD SP, 0x12
        LD HL, 0xF0F0
        PUSH HL
        POP BC
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x12);
    ASSERT_EQ(expected_data.BC.u16, 0xF0F0);
}

// 0xD1
TEST(test_load_16bit, pop_DE)
{
    std::string assembly{R"(
        LD SP, 0x89
        LD HL, 0x1561
        PUSH HL
        POP DE
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x89);
    ASSERT_EQ(expected_data.DE.u16, 0x1561);
}

// 0xE1
TEST(test_load_16bit, pop_HL)
{
    std::string assembly{R"(
        LD SP, 0x99
        LD BC, 0xBABE
        PUSH BC
        POP HL
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x99);
    ASSERT_EQ(expected_data.HL.u16, 0xBABE);
}

// 0xF1
TEST(test_load_16bit, pop_AF)
{
    std::string assembly{R"(
        LD SP, 0x303
        LD DE, 0xBAFE
        PUSH DE
        POP AF
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF1)
            expected_data = d;
    };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x303);
    ASSERT_EQ(expected_data.AF.u16, 0xBAFE);
}