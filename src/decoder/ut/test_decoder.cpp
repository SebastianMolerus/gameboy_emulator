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
    Opcode op = get_opcode(0x01);
    ASSERT_EQ(op.mnemonic, "LD");

    op = get_opcode(0x24);
    ASSERT_EQ(op.mnemonic, "INC");

    op = get_opcode(0x25);
    ASSERT_EQ(op.mnemonic, "DEC");

    op = get_opcode(0x9E);
    ASSERT_EQ(op.mnemonic, "SBC");
}

TEST_F(DecoderTests, bytes)
{
    Opcode op = get_opcode(0x03);
    ASSERT_EQ(op.bytes, 1);

    op = get_opcode(0xCA);
    ASSERT_EQ(op.bytes, 3);

    op = get_opcode(0x06);
    ASSERT_EQ(op.bytes, 2);
}

TEST_F(DecoderTests, cycles)
{
    Opcode op = get_opcode(0x01);
    ASSERT_EQ(op.cycles[0], 12);
    ASSERT_EQ(op.cycles[1], 0xFF);

    op = get_opcode(0x07);
    ASSERT_EQ(op.cycles[0], 4);
    ASSERT_EQ(op.cycles[1], 0xFF);

    op = get_opcode(0x0A);
    ASSERT_EQ(op.cycles[0], 8);
    ASSERT_EQ(op.cycles[1], 0xFF);

    op = get_opcode(0x20);
    ASSERT_EQ(op.cycles[0], 12);
    ASSERT_EQ(op.cycles[1], 8);
}

TEST_F(DecoderTests, immediate)
{
    Opcode op = get_opcode(0x0A);
    ASSERT_FALSE(op.immediate);

    op = get_opcode(0x2A);
    ASSERT_FALSE(op.immediate);

    op = get_opcode(0x0F);
    ASSERT_TRUE(op.immediate);

    op = get_opcode(0x1F);
    ASSERT_TRUE(op.immediate);
}

TEST_F(DecoderTests, operands_A)
{
    Opcode op = get_opcode(0x22);

    Operand const op1 = op.operands[0];
    ASSERT_EQ(op1.name, "HL");
    ASSERT_EQ(op1.increment, 1);
    ASSERT_EQ(op1.immediate, 0);
    ASSERT_EQ(op1.decrement, -1);
    ASSERT_EQ(op1.bytes, -1);

    Operand const op2 = op.operands[1];
    ASSERT_EQ(op2.name, "A");
    ASSERT_EQ(op2.immediate, 1);
    ASSERT_EQ(op2.increment, -1);
    ASSERT_EQ(op2.decrement, -1);
    ASSERT_EQ(op2.bytes, -1);

    ASSERT_EQ(op.operands[2].name, nullptr);
}

TEST_F(DecoderTests, operands_B)
{
    Opcode op = get_opcode(0x32);

    Operand const op1 = op.operands[0];
    ASSERT_EQ(op1.name, "HL");
    ASSERT_EQ(op1.increment, -1);
    ASSERT_EQ(op1.immediate, 0);
    ASSERT_EQ(op1.decrement, 1);
    ASSERT_EQ(op1.bytes, -1);

    Operand const op2 = op.operands[1];
    ASSERT_EQ(op2.name, "A");
    ASSERT_EQ(op2.immediate, 1);
    ASSERT_EQ(op2.increment, -1);
    ASSERT_EQ(op2.decrement, -1);
    ASSERT_EQ(op2.bytes, -1);

    ASSERT_EQ(op.operands[2].name, nullptr);
}

TEST_F(DecoderTests, operands_C)
{
    Opcode op = get_opcode(0x01);

    Operand const op1 = op.operands[0];
    ASSERT_EQ(op1.name, "BC");
    ASSERT_EQ(op1.increment, -1);
    ASSERT_EQ(op1.immediate, 1);
    ASSERT_EQ(op1.decrement, -1);
    ASSERT_EQ(op1.bytes, -1);

    Operand const op2 = op.operands[1];
    ASSERT_EQ(op2.name, "n16");
    ASSERT_EQ(op2.immediate, 1);
    ASSERT_EQ(op2.increment, -1);
    ASSERT_EQ(op2.decrement, -1);
    ASSERT_EQ(op2.bytes, 2);

    ASSERT_EQ(op.operands[2].name, nullptr);
}

TEST_F(DecoderTests, operands_D)
{
    Opcode op = get_opcode(0xF8);

    Operand const op1 = op.operands[0];
    ASSERT_EQ(op1.name, "HL");
    ASSERT_EQ(op1.increment, -1);
    ASSERT_EQ(op1.immediate, 1);
    ASSERT_EQ(op1.decrement, -1);
    ASSERT_EQ(op1.bytes, -1);

    Operand const op2 = op.operands[1];
    ASSERT_EQ(op2.name, "SP");
    ASSERT_EQ(op2.immediate, 1);
    ASSERT_EQ(op2.increment, 1);
    ASSERT_EQ(op2.decrement, -1);
    ASSERT_EQ(op2.bytes, -1);

    Operand const op3 = op.operands[2];
    ASSERT_EQ(op3.name, "e8");
    ASSERT_EQ(op3.immediate, 1);
    ASSERT_EQ(op3.increment, -1);
    ASSERT_EQ(op3.decrement, -1);
    ASSERT_EQ(op3.bytes, 1);
}

TEST_F(DecoderTests, flags)
{
    Opcode op = get_opcode(0xF1);

    ASSERT_EQ(op.flags[0], "Z");
    ASSERT_EQ(op.flags[1], "N");
    ASSERT_EQ(op.flags[2], "H");
    ASSERT_EQ(op.flags[3], "C");

    op = get_opcode(0xF8);

    ASSERT_EQ(op.flags[0], "0");
    ASSERT_EQ(op.flags[1], "0");
    ASSERT_EQ(op.flags[2], "H");
    ASSERT_EQ(op.flags[3], "C");

    op = get_opcode(0x01);

    ASSERT_EQ(op.flags[0], "-");
    ASSERT_EQ(op.flags[1], "-");
    ASSERT_EQ(op.flags[2], "-");
    ASSERT_EQ(op.flags[3], "-");

    op = get_opcode(0x04);

    ASSERT_EQ(op.flags[0], "Z");
    ASSERT_EQ(op.flags[1], "0");
    ASSERT_EQ(op.flags[2], "H");
    ASSERT_EQ(op.flags[3], "-");
}

TEST_F(DecoderTests, loadingCommands)
{
    Opcode op = get_opcode("LD BC, n16");
    ASSERT_EQ(op.hex, 0x01);

    op = get_opcode("NOP");
    ASSERT_EQ(op.hex, 0x00);

    op = get_opcode("AND A, [HL]");
    ASSERT_EQ(op.hex, 0xA6);

    op = get_opcode("LD [HL+], A");
    ASSERT_EQ(op.hex, 0x22);

    op = get_opcode("RLCA");
    ASSERT_EQ(op.hex, 0x07);

    op = get_opcode("LD A, [HL-]");
    ASSERT_EQ(op.hex, 0x3A);

    op = get_opcode("LD B, H");
    ASSERT_EQ(op.hex, 0x44);

    op = get_opcode("PREFIX");
    ASSERT_EQ(op.hex, 0xCB);

    op = get_opcode("RES 6, [HL]");
    ASSERT_EQ(op.hex, 0xB6);

    op = get_opcode("LD A, [HL+]");
    ASSERT_EQ(op.hex, 0x2A);

    op = get_opcode("LD HL, SP + e8");
    ASSERT_EQ(op.hex, 0xF8);

    op = get_opcode("LD BC, n16");
    ASSERT_EQ(op.hex, 0x01);

    op = get_opcode("JR e8");
    ASSERT_EQ(op.hex, 0x18);
}