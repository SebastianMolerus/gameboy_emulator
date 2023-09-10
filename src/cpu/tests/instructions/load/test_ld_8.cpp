#include "cpu.hpp"
#include "program_creator.hpp"
#include <gtest/gtest.h>
#include <translator.hpp>

// 0x02
TEST(test_load_8bit, LD_IBCI_A)
{
    std::string assembly{R"(
        LD BC, 0x59
        LD A, 0x15
        LD [BC], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x59], 0x15);
}

TEST(LoadTest, load_B_C_D_E_H_L_to_A)
{
    program_creator pc;
    pc.ld_BC_nn(0xCE47)
        .ld_DE_nn(0x3099)
        .ld_HL_nn(0xF1F2)
        .ld_reg8_reg8("A", "B")
        .ld_reg8_reg8("A", "C")
        .ld_reg8_reg8("A", "D")
        .ld_reg8_reg8("A", "E")
        .ld_reg8_reg8("A", "H")
        .ld_reg8_reg8("A", "L");
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
    // program_creator pc;
    // // saving value 0x67 at memory location 0x100 using SP
    // // and then read this memory via HL and save it to A
    // pc.ld_SP_nn(0x67).ld_Ia16I_SP(0x100).ld_HL_nn(0x100).ld_A_IHLI();
    // Cpu cpu{pc.get()};
    std::string assembly{R"(
         LD SP, 0x67
         LD [0x100], SP
         LD HL, 0x100
         LD A, [HL]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};
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
    // program_creator pc;
    // pc.ld_SP_nn(0x12).ld_Ia16I_SP(0xACCA).ld_HL_nn(0xACCA).ld_A_IHL_plusI();
    // Cpu cpu{pc.get()};
    std::string assembly{R"(
         LD SP, 0x12
         LD [0xACCA], SP
         LD HL, 0xACCA
         LD A, [HL+]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};
    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == get_opcode("LD A, [HL+]").hex)
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
    pc.ld_SP_nn(0x79).ld_Ia16I_SP(0x02).ld_HL_nn(0x02).ld_A_IHL_minusI();
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
    pc.ld_SP_nn(0x1234).ld_Ia16I_SP(0xFF0A).ld_BC_nn(0xA).ld_A_ICI();
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
TEST(LoadTest, load_n8_to_A)
{
    program_creator pc;
    pc.ld_A_n8(0xE3);
    Cpu cpu{pc.get()};
    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x3E)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.AF.hi, 0xE3);
}
TEST(LoadTest, load_addr_to_A)
{
    program_creator pc;
    pc.ld_SP_nn(0x9876).ld_Ia16I_SP(0x3003).ld_A_Ia16I(0x3003);
    Cpu cpu{pc.get()};
    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xFA)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.AF.hi, 0x76);
}
TEST(LoadTest, load_BC_addr_to_A)
{
    program_creator pc;
    pc.ld_SP_nn(0x3298).ld_Ia16I_SP(0x205).ld_BC_nn(0x205).ld_A_IBCI();
    Cpu cpu{pc.get()};
    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xA)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.AF.hi, 0x98);
}
TEST(LoadTest, load_DE_addr_to_A)
{
    program_creator pc;
    pc.ld_SP_nn(0x11A9).ld_Ia16I_SP(0x0000).ld_DE_nn(0x0).ld_A_IDEI();
    Cpu cpu{pc.get()};
    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x1A)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.AF.hi, 0xA9);
}
TEST(LoadTest, load_n_to_B)
{
    // program_creator pc;
    // pc.ld_B_n8(0x67).ld_B_n8(0x10).ld_B_n8(0xFA);
    // Cpu cpu{pc.get()};
    std::string assembly{R"(
         LD B, 0x67
         LD B, 0x10
         LD B, 0xFA
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};
    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].BC.hi, 0x67);
    ASSERT_EQ(expected_data[1].BC.hi, 0x10);
    ASSERT_EQ(expected_data[2].BC.hi, 0xFA);
}