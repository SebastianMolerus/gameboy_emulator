#include "cpu.hpp"
#include <assembler.hpp>
#include <gtest/gtest.h>
#include <utils.h>

// 0x01
TEST(test_load_16bit, LD_BC_n16)
{
    std::string assembly{R"(
        LD BC, 0x16C7
        LD BC, 0x8
        LD BC, 0x76
        LD BC, 0x5E9
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 12);
                expected_data.push_back(regs);
                if (expected_data.size() == 4)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].BC(), 0x16C7);
    ASSERT_EQ(expected_data[0].C(), 0xC7);
    ASSERT_EQ(expected_data[0].B(), 0x16);

    ASSERT_EQ(expected_data[1].BC(), 0x8);
    ASSERT_EQ(expected_data[1].C(), 0x8);
    ASSERT_EQ(expected_data[1].B(), 0x0);

    ASSERT_EQ(expected_data[2].BC(), 0x76);
    ASSERT_EQ(expected_data[2].C(), 0x76);
    ASSERT_EQ(expected_data[2].B(), 0x0);

    ASSERT_EQ(expected_data[3].BC(), 0x5E9);
    ASSERT_EQ(expected_data[3].C(), 0xE9);
    ASSERT_EQ(expected_data[3].B(), 0x5);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 12);
                expected_data.push_back(regs);
                if (expected_data.size() == 4)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].DE(), 0x157F);
    ASSERT_EQ(expected_data[0].E(), 0x7F);
    ASSERT_EQ(expected_data[0].D(), 0x15);

    ASSERT_EQ(expected_data[1].DE(), 0x0);
    ASSERT_EQ(expected_data[1].E(), 0x0);
    ASSERT_EQ(expected_data[1].D(), 0x0);

    ASSERT_EQ(expected_data[2].DE(), 0x12);
    ASSERT_EQ(expected_data[2].E(), 0x12);
    ASSERT_EQ(expected_data[2].D(), 0x0);

    ASSERT_EQ(expected_data[3].DE(), 0x87F);
    ASSERT_EQ(expected_data[3].E(), 0x7F);
    ASSERT_EQ(expected_data[3].D(), 0x8);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 12);
                expected_data.push_back(regs);
                if (expected_data.size() == 4)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].HL(), 0xF50F);
    ASSERT_EQ(expected_data[0].L(), 0xF);
    ASSERT_EQ(expected_data[0].H(), 0xF5);

    ASSERT_EQ(expected_data[1].HL(), 0xA);
    ASSERT_EQ(expected_data[1].L(), 0xA);
    ASSERT_EQ(expected_data[1].H(), 0x0);

    ASSERT_EQ(expected_data[2].HL(), 0xBA);
    ASSERT_EQ(expected_data[2].L(), 0xBA);
    ASSERT_EQ(expected_data[2].H(), 0x0);

    ASSERT_EQ(expected_data[3].HL(), 0xCBA);
    ASSERT_EQ(expected_data[3].L(), 0xBA);
    ASSERT_EQ(expected_data[3].H(), 0xC);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                assert(wait_cycles == 12);
                expected_data.push_back(regs);
                if (expected_data.size() == 4)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].m_SP.m_u16, 0x7800);
    ASSERT_EQ(expected_data[0].m_SP.m_lo, 0x00);
    ASSERT_EQ(expected_data[0].m_SP.m_hi, 0x78);

    ASSERT_EQ(expected_data[1].m_SP.m_u16, 0xF);
    ASSERT_EQ(expected_data[1].m_SP.m_lo, 0xF);
    ASSERT_EQ(expected_data[1].m_SP.m_hi, 0x0);

    ASSERT_EQ(expected_data[2].m_SP.m_u16, 0x2A);
    ASSERT_EQ(expected_data[2].m_SP.m_lo, 0x2A);
    ASSERT_EQ(expected_data[2].m_SP.m_hi, 0x0);

    ASSERT_EQ(expected_data[3].m_SP.m_u16, 0x87B);
    ASSERT_EQ(expected_data[3].m_SP.m_lo, 0x7B);
    ASSERT_EQ(expected_data[3].m_SP.m_hi, 0x8);
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

    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD HL, SP + e8").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data.push_back(regs);
                }
                if (expected_data.size() == 3)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 3);

    for (auto &data : expected_data)
    {
        ASSERT_FALSE(data.is_flag_set(flag::Z));
        ASSERT_FALSE(data.is_flag_set(flag::N));
    }

    ASSERT_EQ(expected_data[0].HL(), 97);
    ASSERT_EQ(expected_data[0].SP(), 0x3E);
    ASSERT_TRUE(expected_data[0].is_flag_set(flag::H));
    ASSERT_FALSE(expected_data[0].is_flag_set(flag::C));

    ASSERT_EQ(expected_data[1].HL(), 0);
    ASSERT_EQ(expected_data[1].SP(), 0xFFFF);
    ASSERT_TRUE(expected_data[1].is_flag_set(flag::H));
    ASSERT_TRUE(expected_data[1].is_flag_set(flag::C));

    ASSERT_EQ(expected_data[2].SP(), 0x5);
    ASSERT_EQ(expected_data[2].HL(), 4);
}

// // 0xF8
TEST(test_load_16bit, LD_HL_SP_e8_half_carry)
{
    std::string assembly{R"(
        LD SP, 0x9
        LD HL, SP + 0x10
        LD SP, 0xA
        LD HL, SP + 0xC
    )"};

    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD HL, SP + e8").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data.push_back(regs);
                }
                if (expected_data.size() == 2)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 2);

    ASSERT_FALSE(expected_data[0].is_flag_set(flag::H));
    ASSERT_TRUE(expected_data[1].is_flag_set(flag::H));
}

// 0xF8
TEST(test_load_16bit, LD_HL_SP_e8_carry)
{
    std::string assembly{R"(
        LD SP, 0x00A1
        LD HL, SP + 0x1
        LD SP, 0x00FF
        LD HL, SP + 0x1
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("LD HL, SP + e8").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data.push_back(regs);
                }
                if (expected_data.size() == 2)
                    return true;
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.size(), 2);

    ASSERT_FALSE(expected_data[0].is_flag_set(flag::C));
    ASSERT_TRUE(expected_data[1].is_flag_set(flag::C));
}

// 0x08
TEST(test_load_16bit, LD_Ia16I_SP)
{
    std::string assembly{R"(
        LD SP, 0xBBAA
        LD [0x20], SP
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                expected_data.push_back(regs);
                if (expected_data.size() == 2)
                {
                    assert(wait_cycles == 20);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(mock.m_ram[0x20], 0xAA);
    ASSERT_EQ(mock.m_ram[0x21], 0xBB);
}

// 0xF9
TEST(test_load_16bit, LD_SP_HL)
{
    std::string assembly{R"(
        LD HL, 0xABCD
        LD SP, HL
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    std::vector<registers> expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                expected_data.push_back(regs);
                if (expected_data.size() == 2)
                {
                    assert(wait_cycles == 8);
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data[0].HL(), 0xABCD);
    ASSERT_EQ(expected_data[1].SP(), expected_data[1].HL());
}

// // 0xC5
TEST(test_load_16bit, PUSH_BC)
{
    std::string assembly{R"(
        LD SP, 0xFF
        LD BC, 0x548A
        PUSH BC
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("PUSH BC").m_hex)
                {
                    assert(wait_cycles == 16);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP(), 0xFD);
    ASSERT_EQ(mock.m_ram[expected_data.SP()], 0x8A);
    ASSERT_EQ(mock.m_ram[expected_data.SP() + 1], 0x54);
}

// 0xD5
TEST(test_load_16bit, PUSH_DE)
{
    std::string assembly{R"(
        LD SP, 0xFFBB
        LD DE, 0xAAFF
        PUSH DE
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("PUSH DE").m_hex)
                {
                    assert(wait_cycles == 16);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP(), 0xFFB9);
    ASSERT_EQ(mock.m_ram[expected_data.SP()], 0xFF);
    ASSERT_EQ(mock.m_ram[expected_data.SP() + 1], 0xAA);
}

// 0xE5
TEST(test_load_16bit, PUSH_HL)
{
    std::string assembly{R"(
        LD SP, 0xFFAB
        LD HL, 0x1234
        PUSH HL
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("PUSH HL").m_hex)
                {
                    assert(wait_cycles == 16);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP(), 0xFFA9);
    ASSERT_EQ(mock.m_ram[expected_data.SP()], 0x34);
    ASSERT_EQ(mock.m_ram[expected_data.SP() + 1], 0x12);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("PUSH AF").m_hex)
                {
                    assert(wait_cycles == 16);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP(), 0xFE);
    ASSERT_EQ(mock.m_ram[expected_data.SP()], 0x00);
    ASSERT_EQ(mock.m_ram[expected_data.SP() + 1], 0xBD);
}

// 0xC1
TEST(test_load_16bit, pop_BC)
{
    std::string assembly{R"(
        LD SP, 0xFF12
        LD HL, 0xF0F0
        PUSH HL
        POP BC
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("POP BC").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.SP(), 0xFF12);
    ASSERT_EQ(expected_data.BC(), 0xF0F0);
}

// 0xD1
TEST(test_load_16bit, pop_DE)
{
    std::string assembly{R"(
        LD SP, 0xAA89
        LD HL, 0x1561
        PUSH HL
        POP DE
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("POP DE").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.SP(), 0xAA89);
    ASSERT_EQ(expected_data.DE(), 0x1561);
}

// 0xE1
TEST(test_load_16bit, pop_HL)
{
    std::string assembly{R"(
        LD SP, 0xAF99
        LD BC, 0xBABE
        PUSH BC
        POP HL
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("POP HL").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.SP(), 0xAF99);
    ASSERT_EQ(expected_data.HL(), 0xBABE);
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
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == get_opcode("POP AF").m_hex)
                {
                    assert(wait_cycles == 12);
                    expected_data = regs;
                    return true;
                }
                return false;
            }};
    cpu.start();

    ASSERT_EQ(expected_data.SP(), 0x303);
    ASSERT_EQ(expected_data.AF(), 0xBAFE);
}