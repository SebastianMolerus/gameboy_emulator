#include <assembler.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <utils.h>

// 0x02
TEST(test_load_8bit, LD_IBCI_A)
{
    std::string assembly{R"(
        LD BC, 0x59
        LD A, 0x15
        LD [BC], A
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD [BC], A").m_hex)
                {
                    expected_data = regs;
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0x59], 0x15);
}

// 0x12
TEST(test_load_8bit, LD_IDEI_A)
{
    std::string assembly{R"(
        LD DE, 0x789F
        LD A, 0xEA
        LD [DE], A
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD [DE], A").m_hex)
                {
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0x789F], 0xEA);
}

// 0x22
TEST(test_load_8bit, LD_IHLplusI_A)
{
    std::string assembly{R"(
        LD HL, 0x9998
        LD A, 0x61
        LD [HL+], A
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD [HL+], A").m_hex)
                {
                    expected_data = regs;
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0x9998], 0x61);
    ASSERT_EQ(expected_data.HL(), 0x9999);
}

// 0x32
TEST(test_load_8bit, LD_IHLminusI_A)
{
    std::string assembly{R"(
        LD HL, 0x50
        LD A, 0x51
        LD [HL-], A
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD [HL-], A").m_hex)
                {
                    expected_data = regs;
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0x50], 0x51);
    ASSERT_EQ(expected_data.HL(), 0x4F);
}

// 0xE0
TEST(test_load_8bit, LDH_Ia8I_A)
{
    std::string assembly{R"(
        LD A, 0x99
        LDH [0x15], A
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LDH [a8], A").m_hex)
                {
                    assert(wait_cycles == 12);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0xFF00 + 0x15], 0x99);
}

// 0xF0
TEST(test_load_8bit, LDH_A_Ia8I)
{
    std::string assembly{R"(
        LD SP, 0x26
        LD [0xFF87], SP
        LDH A, [0x87]
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LDH A, [a8]").m_hex)
                {
                    expected_data = regs;
                    assert(wait_cycles == 12);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x26);
}

// 0x06
TEST(test_load_8bit, LD_B_n8)
{
    std::string assembly{R"(
         LD B, 0x07
         LD B, 0x1
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 2)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 2);
    ASSERT_EQ(expected_data[0].B(), 0x07);
    ASSERT_EQ(expected_data[1].B(), 0x1);
}

// 0x16
TEST(test_load_8bit, LD_D_n8)
{
    std::string assembly{R"(
         LD D, 0xAA
         LD D, 0xB
         LD D, 0x0
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].D(), 0xAA);
    ASSERT_EQ(expected_data[1].D(), 0xB);
    ASSERT_EQ(expected_data[2].D(), 0x0);
}

// 0x26
TEST(test_load_8bit, LD_H_n8)
{
    std::string assembly{R"(
         LD H, 0x12
         LD H, 0x59
         LD H, 0x4
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].H(), 0x12);
    ASSERT_EQ(expected_data[1].H(), 0x59);
    ASSERT_EQ(expected_data[2].H(), 0x4);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                static int instruction_cc{};
                ++instruction_cc;
                if (get_opcode("LD [HL], n8").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 12);
                }

                if (instruction_cc == 4)
                    return true;

                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0xFF32], 0x8);
    ASSERT_EQ(mock.m_ram[0x784E], 0xF6);
}

// 0x0E
TEST(test_load_8bit, LD_C_n8)
{
    std::string assembly{R"(
         LD C, 0x1
         LD C, 0x01
         LD C, 0xF5
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].C(), 0x1);
    ASSERT_EQ(expected_data[1].C(), 0x01);
    ASSERT_EQ(expected_data[2].C(), 0xF5);
}

// 0x1E
TEST(test_load_8bit, LD_E_n8)
{
    std::string assembly{R"(
         LD E, 0x87
         LD E, 0xA4
         LD E, 0x8D
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].E(), 0x87);
    ASSERT_EQ(expected_data[1].E(), 0xA4);
    ASSERT_EQ(expected_data[2].E(), 0x8D);
}

// 0x2E
TEST(test_load_8bit, LD_L_n8)
{
    std::string assembly{R"(
         LD L, 0x72
         LD L, 0x71
         LD L, 0x0F
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].L(), 0x72);
    ASSERT_EQ(expected_data[1].L(), 0x71);
    ASSERT_EQ(expected_data[2].L(), 0x0F);
}

// 0x3E
TEST(test_load_8bit, LD_A_n8)
{
    std::string assembly{R"(
         LD A, 0x0
         LD A, 0x2
         LD A, 0x3
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 8);
                expected_data.push_back(regs);
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].A(), 0x0);
    ASSERT_EQ(expected_data[1].A(), 0x2);
    ASSERT_EQ(expected_data[2].A(), 0x3);
}

// 0x0A
TEST(test_load_8bit, LD_A_IBCI)
{
    std::string assembly{R"(
         LD SP, 0x8510
         LD [0x667], SP
         LD BC, 0x667
         LD A, [BC]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD A, [BC]").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 8);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x10);
    ASSERT_EQ(mock.m_ram[0x667], 0x10);
    ASSERT_EQ(mock.m_ram[0x667 + 1], 0x85);
}

// 0x1A
TEST(test_load_8bit, LD_A_IDEI)
{
    std::string assembly{R"(
         LD SP, 0xAFAB
         LD [0x0099], SP
         LD DE, 0x0099
         LD A, [DE]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD A, [DE]").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 8);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0xAB);
    ASSERT_EQ(mock.m_ram[0x0099], 0xAB);
    ASSERT_EQ(mock.m_ram[0x0099 + 1], 0xAF);
}

// 0x2A
TEST(test_load_8bit, LD_A_IHLplusI)
{
    std::string assembly{R"(
         LD SP, 0x1234
         LD [0xFF01], SP
         LD HL, 0xFF01
         LD A, [HL+]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD A, [HL+]").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 8);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x34);
    ASSERT_EQ(mock.m_ram[0xFF01], 0x34);
    ASSERT_EQ(mock.m_ram[0xFF01 + 1], 0x12);
    ASSERT_EQ(expected_data.HL(), 0xFF01 + 1);
}

// 0x3A
TEST(test_load_8bit, LD_A_IHLminusI)
{
    std::string assembly{R"(
         LD SP, 0x1234
         LD [0xFF01], SP
         LD HL, 0xFF01
         LD A, [HL+]
         LD A, [HL-]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD A, [HL-]").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 8);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x12);
    ASSERT_EQ(mock.m_ram[0xFF01], 0x34);
    ASSERT_EQ(mock.m_ram[0xFF01 + 1], 0x12);
    ASSERT_EQ(expected_data.HL(), 0xFF01);
}

// 0xEA
TEST(test_load_8bit, LD_Ia16I_A)
{
    std::string assembly{R"(
         LD A, 0xAC
         LD [0xDEC5], A
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD [a16], A").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 16);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0xDEC5], 0xAC);
}

// 0xFA
TEST(test_load_8bit, LD_A_Ia16I)
{
    std::string assembly{R"(
         LD A, [0xAB43]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    mock.m_ram[0xAB43] = 0x67;
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                expected_data = regs;
                assert(wait_cycles == 16);
                return true;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x67);
}

// 0xE2
TEST(test_load_8bit, LD_ICI_A)
{
    std::string assembly{R"(
         LD A, 0x71
         LD C, 0x39
         LD [C], A
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD [C], A").m_hex == op.m_hex)
                {
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0xFF39], 0x71);
}

// 0xF2
TEST(test_load_8bit, LD_A_ICI)
{
    std::string assembly{R"(
         LD C, 0xEB
         LD A, [C]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    mock.m_ram[0xFFEB] = 0x8E;
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (get_opcode("LD A, [C]").m_hex == op.m_hex)
                {
                    expected_data = regs;
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.A(), 0x8E);
}

// 0x70 0x71 0x72 0x73 0x77
TEST(test_load_8bit, LD_IHLI_reg8)
{
    std::string assembly{R"(
        LD B, 0x1
        LD C, 0x2
        LD D, 0x3
        LD E, 0x4
        LD A, 0x5
        LD HL, 0xF1F2
        LD [HL], B
        LD [HL], C
        LD [HL], D
        LD [HL], E
        LD [HL], A
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<uint8_t> expected_data;
    cpu cpu{mock, [&expected_data, &mock](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex >= 0x70 && op.m_hex <= 0x77)
                {
                    assert(wait_cycles == 8);
                    expected_data.push_back(mock.m_ram[0xF1F2]);
                }

                if (get_opcode("LD [HL], A").m_hex == op.m_hex)
                    return true;

                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data[0], 0x1);
    ASSERT_EQ(expected_data[1], 0x2);
    ASSERT_EQ(expected_data[2], 0x3);
    ASSERT_EQ(expected_data[3], 0x4);
    ASSERT_EQ(expected_data[4], 0x5);
}

// 0x74 0x75
TEST(test_load_8bit, LD_IHLI_H_or_L)
{
    std::string assembly{R"(
        LD HL, 0x659D
        LD [HL], L
        LD HL, 0x659D
        LD [HL], H
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<uint8_t> expected_data;
    cpu cpu{mock, [&expected_data, &mock](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex >= 0x74 && op.m_hex <= 0x75)
                {
                    assert(wait_cycles == 8);
                    expected_data.push_back(mock.m_ram[0x659D]);
                }

                if (get_opcode("LD [HL], H").m_hex == op.m_hex)
                    return true;

                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 2);
    ASSERT_EQ(expected_data[0], 0x9D);
    ASSERT_EQ(expected_data[1], 0x65);
}

// 0x46 0x56 0x66 0x4E 0x5E 0x6E 0x7E
TEST(test_load_8bit, LD_REG8_IHLI_)
{
    std::string assembly{R"(
        LD HL, 0x1234
        LD B, [HL]
        LD D, [HL]
        LD H, [HL]
        LD C, [HL]
        LD E, [HL]
        LD L, [HL]
        LD A, [HL]
     )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    mock.m_ram[0x1234] = 0x5F;
    mock.m_ram[0x5F34] = 0x66;
    mock.m_ram[0x5F66] = 0x09;

    std::vector<uint8_t> expected_data;
    cpu cpu{mock, [&expected_data, &mock](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex != 0x21)
                {
                    assert(wait_cycles == 8);
                    expected_data.push_back(regs.get_byte(op.m_operands[0].m_name));
                }

                if (get_opcode("LD A, [HL]").m_hex == op.m_hex)
                    return true;

                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 7);

    ASSERT_EQ(expected_data[0], 0x5F);
    ASSERT_EQ(expected_data[1], 0x5F);
    ASSERT_EQ(expected_data[2], 0x5F);

    // 0x5F34
    ASSERT_EQ(expected_data[3], 0x66);
    ASSERT_EQ(expected_data[4], 0x66);
    ASSERT_EQ(expected_data[5], 0x66);

    // 0x5F66
    ASSERT_EQ(expected_data[6], 0x09);
}

// Combination of loads from [A B C D E H L] registers to [A B C D E H L] registers
// 0x40 0x41 0x42 0x43 0x44 0x45
// 0x50 0x51 0x52 0x53 0x54 0x55
// 0x60 0x61 0x62 0x63 0x64 0x65
// 0x47 0x48 0x49 0x4A 0x4B 0x4C 0x4D
// 0x57 0x58 0x59 0x5A 0x5B 0x5C 0x5D
// 0x67 0x68 0x69 0x6A 0x6B 0x6C 0x6D
// 0x78 0x79 0x7A 0x7B 0x7C 0x7D
// 0x4F 0x5F 0x6F 0x7F
uint8_t test(std::span<uint8_t> opcodes, uint8_t source_value)
{
    rw_mock mock{opcodes};
    uint8_t expected_data;
    int instruction_cc{};
    cpu cpu{mock,
            [&expected_data, &mock, &instruction_cc](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                ++instruction_cc;
                if (instruction_cc == 2)
                {
                    expected_data = regs.get_byte(op.m_operands[0].m_name);
                    return true;
                }
                else
                    return false;
            }};
    cpu.start();
    return expected_data;
}

TEST(test_load_8bit, LD_REG8_REG8)
{
    srand((unsigned)time(NULL));
    std::vector<uint8_t> opcodes_to_cover{0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x60,
                                          0x61, 0x62, 0x63, 0x64, 0x65, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x57,
                                          0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
                                          0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x4F, 0x5F, 0x6F, 0x7F};

    for (auto source : {'A', 'B', 'C', 'D', 'E', 'H', 'L'})
        for (auto destination : {'A', 'B', 'C', 'D', 'E', 'H', 'L'})
        {

            int source_value = rand();
            source_value &= 0xFF;
            std::stringstream ss;
            ss << "0x" << std::hex << source_value;

            std::string cmd = "LD ";
            cmd += source;
            cmd += ", ";
            cmd += ss.str();
            cmd += '\n';

            cmd += "LD ";
            cmd += destination;
            cmd += ", ";
            cmd += source;

            auto opcodes = transform(cmd);
            ASSERT_EQ(opcodes.size(), 3);

            auto dest_value = test(opcodes, source_value);

            ASSERT_EQ(dest_value, source_value) << "DEBUG INFO: " << cmd;

            opcodes_to_cover.erase(remove(opcodes_to_cover.begin(), opcodes_to_cover.end(), opcodes[2]),
                                   opcodes_to_cover.end());
        }

    ASSERT_EQ(opcodes_to_cover.size(), 0);
}