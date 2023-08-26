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
    Opcode op;
    ASSERT_TRUE(get_opcode("0x01", op));
    ASSERT_EQ(op.mnemonic, "LD");

    ASSERT_TRUE(get_opcode("0x24", op));
    ASSERT_EQ(op.mnemonic, "INC");

    ASSERT_TRUE(get_opcode("0x25", op));
    ASSERT_EQ(op.mnemonic, "DEC");

    ASSERT_TRUE(get_opcode("0x9E", op));
    ASSERT_EQ(op.mnemonic, "SBC");
}

TEST_F(DecoderTests, bytes)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x03", op));
    ASSERT_EQ(op.bytes, 1);

    ASSERT_TRUE(get_opcode("0xCA", op));
    ASSERT_EQ(op.bytes, 3);

    ASSERT_TRUE(get_opcode("0x06", op));
    ASSERT_EQ(op.bytes, 2);
}

TEST_F(DecoderTests, cycles)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x01", op));
    ASSERT_EQ(op.cycles[0], 12);

    ASSERT_TRUE(get_opcode("0x07", op));
    ASSERT_EQ(op.cycles[0], 4);

    ASSERT_TRUE(get_opcode("0x0A", op));
    ASSERT_EQ(op.cycles[0], 8);

    ASSERT_TRUE(get_opcode("0x20", op));
    ASSERT_EQ(op.cycles[0], 12);
    ASSERT_EQ(op.cycles[1], 8);
}

TEST_F(DecoderTests, immediate)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x0A", op));
    ASSERT_FALSE(op.immediate);

    ASSERT_TRUE(get_opcode("0x2A", op));
    ASSERT_FALSE(op.immediate);

    ASSERT_TRUE(get_opcode("0x0F", op));
    ASSERT_TRUE(op.immediate);

    ASSERT_TRUE(get_opcode("0x1F", op));
    ASSERT_TRUE(op.immediate);
}

TEST_F(DecoderTests, operands_A)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x22", op));

    ASSERT_EQ(op.operands.size(), 2);

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
}

TEST_F(DecoderTests, operands_B)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x32", op));

    ASSERT_EQ(op.operands.size(), 2);

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
}

TEST_F(DecoderTests, operands_C)
{
    Opcode op;
    ASSERT_TRUE(get_opcode("0x01", op));

    ASSERT_EQ(op.operands.size(), 2);

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
}