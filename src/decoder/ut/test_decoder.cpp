#include <gtest/gtest.h>

#include <decoder.hpp>

// https://gbdev.io/gb-opcodes/Opcodes.json
namespace
{
bool opcodes_loaded{};
}

class DecoderTests : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        if (!opcodes_loaded)
        {
            opcodes_loaded = load_opcodes();
            ASSERT_TRUE(opcodes_loaded);
        }
    }
};

TEST_F(DecoderTests, mnemonic_check)
{
    opcode op = get_opcode(0x01);
    ASSERT_EQ(op.m_mnemonic, "LD");

    op = get_opcode(0x24);
    ASSERT_EQ(op.m_mnemonic, "INC");

    op = get_opcode(0x25);
    ASSERT_EQ(op.m_mnemonic, "DEC");

    op = get_opcode(0x9E);
    ASSERT_EQ(op.m_mnemonic, "SBC");
}

TEST_F(DecoderTests, bytes)
{
    opcode op = get_opcode(0x03);
    ASSERT_EQ(op.m_bytes, 1);

    op = get_opcode(0xCA);
    ASSERT_EQ(op.m_bytes, 3);

    op = get_opcode(0x06);
    ASSERT_EQ(op.m_bytes, 2);
}

TEST_F(DecoderTests, cycles)
{
    opcode op = get_opcode(0x01);
    ASSERT_EQ(op.m_cycles[0], 12);
    ASSERT_EQ(op.m_cycles[1], 0x0);

    op = get_opcode(0x07);
    ASSERT_EQ(op.m_cycles[0], 4);
    ASSERT_EQ(op.m_cycles[1], 0x0);

    op = get_opcode(0x0A);
    ASSERT_EQ(op.m_cycles[0], 8);
    ASSERT_EQ(op.m_cycles[1], 0x0);

    op = get_opcode(0x20);
    ASSERT_EQ(op.m_cycles[0], 12);
    ASSERT_EQ(op.m_cycles[1], 8);
}

TEST_F(DecoderTests, immediate)
{
    opcode op = get_opcode(0x0A);
    ASSERT_FALSE(op.m_immediate);

    op = get_opcode(0x2A);
    ASSERT_FALSE(op.m_immediate);

    op = get_opcode(0x0F);
    ASSERT_TRUE(op.m_immediate);

    op = get_opcode(0x1F);
    ASSERT_TRUE(op.m_immediate);
}

TEST_F(DecoderTests, operands_A)
{
    opcode op = get_opcode(0x22);

    operand const op1 = op.m_operands[0];
    ASSERT_EQ(op1.m_name, "HL");
    ASSERT_EQ(op1.m_increment, 1);
    ASSERT_EQ(op1.m_immediate, 0);
    ASSERT_EQ(op1.m_decrement, -1);
    ASSERT_EQ(op1.m_bytes, -1);

    operand const op2 = op.m_operands[1];
    ASSERT_EQ(op2.m_name, "A");
    ASSERT_EQ(op2.m_immediate, 1);
    ASSERT_EQ(op2.m_increment, -1);
    ASSERT_EQ(op2.m_decrement, -1);
    ASSERT_EQ(op2.m_bytes, -1);

    ASSERT_EQ(op.m_operands[2].m_name, nullptr);
}

TEST_F(DecoderTests, operands_B)
{
    opcode op = get_opcode(0x32);

    operand const op1 = op.m_operands[0];
    ASSERT_EQ(op1.m_name, "HL");
    ASSERT_EQ(op1.m_increment, -1);
    ASSERT_EQ(op1.m_immediate, 0);
    ASSERT_EQ(op1.m_decrement, 1);
    ASSERT_EQ(op1.m_bytes, -1);

    operand const op2 = op.m_operands[1];
    ASSERT_EQ(op2.m_name, "A");
    ASSERT_EQ(op2.m_immediate, 1);
    ASSERT_EQ(op2.m_increment, -1);
    ASSERT_EQ(op2.m_decrement, -1);
    ASSERT_EQ(op2.m_bytes, -1);

    ASSERT_EQ(op.m_operands[2].m_name, nullptr);
}

TEST_F(DecoderTests, operands_C)
{
    opcode op = get_opcode(0x01);

    operand const op1 = op.m_operands[0];
    ASSERT_EQ(op1.m_name, "BC");
    ASSERT_EQ(op1.m_increment, -1);
    ASSERT_EQ(op1.m_immediate, 1);
    ASSERT_EQ(op1.m_decrement, -1);
    ASSERT_EQ(op1.m_bytes, -1);

    operand const op2 = op.m_operands[1];
    ASSERT_EQ(op2.m_name, "n16");
    ASSERT_EQ(op2.m_immediate, 1);
    ASSERT_EQ(op2.m_increment, -1);
    ASSERT_EQ(op2.m_decrement, -1);
    ASSERT_EQ(op2.m_bytes, 2);

    ASSERT_EQ(op.m_operands[2].m_name, nullptr);
}

TEST_F(DecoderTests, operands_D)
{
    opcode op = get_opcode(0xF8);

    operand const op1 = op.m_operands[0];
    ASSERT_EQ(op1.m_name, "HL");
    ASSERT_EQ(op1.m_increment, -1);
    ASSERT_EQ(op1.m_immediate, 1);
    ASSERT_EQ(op1.m_decrement, -1);
    ASSERT_EQ(op1.m_bytes, -1);

    operand const op2 = op.m_operands[1];
    ASSERT_EQ(op2.m_name, "SP");
    ASSERT_EQ(op2.m_immediate, 1);
    ASSERT_EQ(op2.m_increment, 1);
    ASSERT_EQ(op2.m_decrement, -1);
    ASSERT_EQ(op2.m_bytes, -1);

    operand const op3 = op.m_operands[2];
    ASSERT_EQ(op3.m_name, "e8");
    ASSERT_EQ(op3.m_immediate, 1);
    ASSERT_EQ(op3.m_increment, -1);
    ASSERT_EQ(op3.m_decrement, -1);
    ASSERT_EQ(op3.m_bytes, 1);
}

TEST_F(DecoderTests, flags)
{
    opcode op = get_opcode(0xF1);

    ASSERT_EQ(op.m_flags[0], "Z");
    ASSERT_EQ(op.m_flags[1], "N");
    ASSERT_EQ(op.m_flags[2], "H");
    ASSERT_EQ(op.m_flags[3], "C");

    op = get_opcode(0xF8);

    ASSERT_EQ(op.m_flags[0], "0");
    ASSERT_EQ(op.m_flags[1], "0");
    ASSERT_EQ(op.m_flags[2], "H");
    ASSERT_EQ(op.m_flags[3], "C");

    op = get_opcode(0x01);

    ASSERT_EQ(op.m_flags[0], "-");
    ASSERT_EQ(op.m_flags[1], "-");
    ASSERT_EQ(op.m_flags[2], "-");
    ASSERT_EQ(op.m_flags[3], "-");

    op = get_opcode(0x04);

    ASSERT_EQ(op.m_flags[0], "Z");
    ASSERT_EQ(op.m_flags[1], "0");
    ASSERT_EQ(op.m_flags[2], "H");
    ASSERT_EQ(op.m_flags[3], "-");
}

TEST_F(DecoderTests, loadingCommands)
{
    opcode op = get_opcode("LD BC, n16");
    ASSERT_EQ(op.m_hex, 0x01);

    op = get_opcode("NOP");
    ASSERT_EQ(op.m_hex, 0x00);

    op = get_opcode("AND A, [HL]");
    ASSERT_EQ(op.m_hex, 0xA6);

    op = get_opcode("LD [HL+], A");
    ASSERT_EQ(op.m_hex, 0x22);

    op = get_opcode("RLCA");
    ASSERT_EQ(op.m_hex, 0x07);

    op = get_opcode("LD A, [HL-]");
    ASSERT_EQ(op.m_hex, 0x3A);

    op = get_opcode("LD B, H");
    ASSERT_EQ(op.m_hex, 0x44);

    op = get_opcode("PREFIX");
    ASSERT_EQ(op.m_hex, 0xCB);

    op = get_opcode("RES 6, [HL]");
    ASSERT_EQ(op.m_hex, 0xB6);

    op = get_opcode("LD A, [HL+]");
    ASSERT_EQ(op.m_hex, 0x2A);

    op = get_opcode("LD HL, SP + e8");
    ASSERT_EQ(op.m_hex, 0xF8);

    op = get_opcode("LD BC, n16");
    ASSERT_EQ(op.m_hex, 0x01);

    op = get_opcode("JR e8");
    ASSERT_EQ(op.m_hex, 0x18);
}