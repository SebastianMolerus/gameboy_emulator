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

// 0x12
TEST(test_load_8bit, LD_IDEI_A)
{
    std::string assembly{R"(
        LD DE, 0x789F
        LD A, 0xEA
        LD [DE], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x789F], 0xEA);
}

// 0x22
TEST(test_load_8bit, LD_IHLplusI_A)
{
    std::string assembly{R"(
        LD HL, 0x9998
        LD A, 0x61
        LD [HL+], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x9998], 0x61);
    ASSERT_EQ(expected_data.HL.u16, 0x9999);
}

// 0x32
TEST(test_load_8bit, LD_IHLminusI_A)
{
    std::string assembly{R"(
        LD HL, 0x5
        LD A, 0x51
        LD [HL-], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x5], 0x51);
    ASSERT_EQ(expected_data.HL.u16, 0x4);
}

// 0xE0
TEST(test_load_8bit, LDH_Ia8I_A)
{
    std::string assembly{R"(
        LD A, 0x99
        LDH [0x15], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0xFF00 + 0x15], 0x99);
}

// 0xF0
TEST(test_load_8bit, LDH_A_Ia8I)
{
    std::string assembly{R"(
        LD SP, 0x26
        LD [0xFF87], SP
        LDH A, [0x87]
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x26);
}

// 0x06
TEST(test_load_8bit, LD_B_n8)
{
    std::string assembly{R"(
         LD B, 0x07
         LD B, 0x1
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 2);
    ASSERT_EQ(expected_data[0].BC.hi, 0x07);
    ASSERT_EQ(expected_data[1].BC.hi, 0x1);
}

// 0x16
TEST(test_load_8bit, LD_D_n8)
{
    std::string assembly{R"(
         LD D, 0xAA
         LD D, 0xB
         LD D, 0x0
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].DE.hi, 0xAA);
    ASSERT_EQ(expected_data[1].DE.hi, 0xB);
    ASSERT_EQ(expected_data[2].DE.hi, 0x0);
}

// 0x26
TEST(test_load_8bit, LD_H_n8)
{
    std::string assembly{R"(
         LD H, 0x12
         LD H, 0x59
         LD H, 0x4
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].HL.hi, 0x12);
    ASSERT_EQ(expected_data[1].HL.hi, 0x59);
    ASSERT_EQ(expected_data[2].HL.hi, 0x4);
}

// 0x36
TEST(test_load_8bit, LD_IHLI_n8)
{
    std::string assembly{R"(
         LD HL, 0xFF32
         LD [HL], 0x8
         LD HL, 0x784E
         LD [HL], 0xF6
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0xFF32], 0x8);
    ASSERT_EQ(expected_data.m_memory[0x784E], 0xF6);
}

// 0x0E
TEST(test_load_8bit, LD_C_n8)
{
    std::string assembly{R"(
         LD C, 0x1
         LD C, 0x01
         LD C, 0xF5
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].BC.lo, 0x1);
    ASSERT_EQ(expected_data[1].BC.lo, 0x01);
    ASSERT_EQ(expected_data[2].BC.lo, 0xF5);
}

// 0x1E
TEST(test_load_8bit, LD_E_n8)
{
    std::string assembly{R"(
         LD E, 0x87
         LD E, 0xA4
         LD E, 0x8D
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].DE.lo, 0x87);
    ASSERT_EQ(expected_data[1].DE.lo, 0xA4);
    ASSERT_EQ(expected_data[2].DE.lo, 0x8D);
}

// 0x2E
TEST(test_load_8bit, LD_L_n8)
{
    std::string assembly{R"(
         LD L, 0x72
         LD L, 0x71
         LD L, 0x0F
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].HL.lo, 0x72);
    ASSERT_EQ(expected_data[1].HL.lo, 0x71);
    ASSERT_EQ(expected_data[2].HL.lo, 0x0F);
}

// 0x3E
TEST(test_load_8bit, LD_A_n8)
{
    std::string assembly{R"(
         LD A, 0x0
         LD A, 0x2
         LD A, 0x3
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].AF.hi, 0x0);
    ASSERT_EQ(expected_data[1].AF.hi, 0x2);
    ASSERT_EQ(expected_data[2].AF.hi, 0x3);
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

// 0xF2
TEST(test_load_8bit, LD_A_ICI)
{
    std::string assembly{R"(
         LD SP, 0x1234
         LD [0xFF0A], SP
         LD BC, 0xA
         LD A, [C]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.AF.hi, 0x34);
}

// 0xE2
TEST(test_load_8bit, LD_ICI_A)
{
    std::string assembly{R"(
         LD A, 0x97
         LD C, 0x4F
         LD [C], A
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();
    ASSERT_EQ(expected_data.m_memory[0xFF00 + 0x4F], 0x97);
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

TEST(LoadTest, LD_B_E)
{
    // program_creator pc;
    // pc.ld_B_n8(0x67).ld_B_n8(0x10).ld_B_n8(0xFA);
    // Cpu cpu{pc.get()};
    std::string assembly{R"(
         LD DE, 0xFF77
         LD B, E
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};
    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data[1].BC.hi, 0x77);
}