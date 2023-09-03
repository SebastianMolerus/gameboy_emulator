#include "cpu.hpp"
#include "program_creator.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <span>
#include <vector>

uint16_t read_word_from_stack(CpuData const &data)
{
    uint16_t result = data.m_memory[data.SP.u16 + 1];
    result <<= 8;
    result |= data.m_memory[data.SP.u16];
    return result;
}

TEST(LoadTest, ld_BC_n16)
{
    uint16_t constexpr value_to_load{0x16C7};
    program_creator pc;
    pc.load_to_BC(value_to_load);
    Cpu cpu{pc.get()};

    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.BC.u16, value_to_load);
}

TEST(LoadTest, ld_DE_n16)
{
    uint16_t constexpr value_to_load{0x157F};
    program_creator pc;
    pc.load_to_DE(value_to_load);
    Cpu cpu{pc.get()};

    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.DE.u16, value_to_load);
}

TEST(LoadTest, ld_HL_n16)
{
    uint16_t constexpr value_to_load{0xF50F};
    program_creator pc;
    pc.load_to_HL(value_to_load);
    Cpu cpu{pc.get()};

    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.HL.u16, value_to_load);
}

TEST(LoadTest, ld_SP_n16)
{
    uint16_t constexpr value_to_load{0xABCD};
    program_creator pc;
    pc.load_to_SP(value_to_load);
    Cpu cpu{pc.get()};

    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.SP.u16, value_to_load);
}

TEST(LoadTest, ld_HL_SP_n8)
{
    program_creator pc;
    pc.load_to_SP(0x3E).add_to_SP(0x23).load_to_SP(0xFFFF).add_to_SP(0x1).load_to_SP(0x5).add_to_SP(0x81);
    Cpu cpu{pc.get()};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8) // after each add_to_SP
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
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

TEST(LoadTest, ld_HL_SP_n8_half_carry)
{
    program_creator pc;
    pc.load_to_SP(0x9).add_to_SP(0x10).load_to_SP(0xA).add_to_SP(0xC);
    Cpu cpu{pc.get()};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8) // after each add_to_SP
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
}

TEST(LoadTest, ld_HL_SP_n8_carry)
{
    program_creator pc;
    pc.load_to_SP(0xFFFE).add_to_SP(0x1).load_to_SP(0xFFFF).add_to_SP(0x1);
    Cpu cpu{pc.get()};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8) // after each add_to_SP
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));
}

TEST(LoadTest, ld_a16_SP)
{
    program_creator pc;
    pc.load_to_SP(0xBBAA).save_SP(0x1);
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x08)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x1], 0xAA);
    ASSERT_EQ(expected_data.m_memory[0x2], 0xBB);
}

TEST(LoadTest, ld_sp_hl)
{
    program_creator pc;
    pc.load_to_HL(0xABCD).load_HL_to_SP();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF9)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.HL.u16, 0xABCD);
    ASSERT_EQ(expected_data.SP.u16, expected_data.HL.u16);
}

TEST(LoadTest, push_BC)
{
    program_creator pc;
    pc.load_to_SP(0xA).load_to_BC(0xFFAA).push_BC();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xC5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 8);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0xAA);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0xFF);
}

TEST(LoadTest, push_DE)
{
    program_creator pc;
    pc.load_to_SP(0xBB).load_to_DE(0xAAFF).push_DE();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xD5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xBB - 2);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0xFF);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0xAA);
}

TEST(LoadTest, push_HL)
{
    program_creator pc;
    pc.load_to_SP(0xAB).load_to_HL(0x1234).push_HL();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xE5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xAB - 2);
    ASSERT_EQ(read_word_from_stack(expected_data), 0x1234);
}

TEST(LoadTest, push_AF)
{
    program_creator pc;
    pc.load_to_SP(0x100).load_to_DE(0xBD00).load_D_to_A().push_AF();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0x0100 - 2);
    ASSERT_EQ(read_word_from_stack(expected_data), 0xBD00);
}

TEST(LoadTest, pop_AF)
{
    program_creator pc;
    pc.load_to_SP(0x303).load_to_DE(0xBAFE).push_DE().pop_AF();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x303);
    ASSERT_EQ(expected_data.AF.u16, 0xBAFE);
}

TEST(LoadTest, pop_BC)
{
    program_creator pc;
    pc.load_to_SP(0x12).load_to_HL(0xF0F0).push_HL().pop_BC();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xC1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x12);
    ASSERT_EQ(expected_data.BC.u16, 0xF0F0);
}

TEST(LoadTest, pop_DE)
{
    program_creator pc;
    pc.load_to_SP(0x89).load_to_HL(0x1561).push_HL().pop_DE();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xD1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x89);
    ASSERT_EQ(expected_data.DE.u16, 0x1561);
}

TEST(LoadTest, pop_HL)
{
    program_creator pc;
    pc.load_to_SP(0x99).load_to_BC(0xBABE).push_BC().pop_HL();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xE1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0x99);
    ASSERT_EQ(expected_data.HL.u16, 0xBABE);
}

TEST(LoadTest, load_B_C_D_E_H_L_to_A)
{
    program_creator pc;
    pc.load_to_BC(0xCE47)
        .load_to_DE(0x3099)
        .load_to_HL(0xF1F2)
        .load_B_to_A()
        .load_C_to_A()
        .load_D_to_A()
        .load_E_to_A()
        .load_H_to_A()
        .load_L_to_A();
    Cpu cpu{pc.get()};

    CpuData B_to_A;
    CpuData C_to_A;
    CpuData D_to_A;
    CpuData E_to_A;
    CpuData H_to_A;
    CpuData L_to_A;
    auto f = [&](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x78)
            B_to_A = d;
        if (op.hex == 0x79)
            C_to_A = d;
        if (op.hex == 0x7A)
            D_to_A = d;
        if (op.hex == 0x7B)
            E_to_A = d;
        if (op.hex == 0x7C)
            H_to_A = d;
        if (op.hex == 0x7D)
            L_to_A = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(B_to_A.AF.hi, 0xCE);
    ASSERT_EQ(C_to_A.AF.hi, 0x47);
    ASSERT_EQ(D_to_A.AF.hi, 0x30);
    ASSERT_EQ(E_to_A.AF.hi, 0x99);
    ASSERT_EQ(H_to_A.AF.hi, 0xF1);
    ASSERT_EQ(L_to_A.AF.hi, 0xF2);
}

TEST(LoadTest, load_HL_to_A)
{
    program_creator pc;
    // saving value 0x67 at memory location 0x100 using SP
    // and then read this memory via HL and save it to A
    pc.load_to_SP(0x67).save_SP(0x100).load_to_HL(0x100).load_HL_to_A();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x7E)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x67);
}

TEST(LoadTest, load_HL_plus_to_A)
{
    program_creator pc;
    pc.load_to_SP(0x12).save_SP(0xACCA).load_to_HL(0xACCA).load_HL_plus_to_A();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x2A)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x12);
    ASSERT_EQ(expected_data.HL.u16, 0xACCB);
}

TEST(LoadTest, load_HL_minus_to_A)
{
    program_creator pc;
    pc.load_to_SP(0x79).save_SP(0x02).load_to_HL(0x02).load_HL_minus_to_A();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x3A)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x79);
    ASSERT_EQ(expected_data.HL.u16, 0x1);
}

TEST(LoadTest, load_C_addr_to_A)
{
    program_creator pc;
    pc.load_to_SP(0x1234).save_SP(0xFF0A).load_to_BC(0xA).load_C_addr_to_A();
    Cpu cpu{pc.get()};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF2)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x34);
}
