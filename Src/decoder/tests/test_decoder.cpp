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
    get_opcode("0x01", op);
    ASSERT_EQ(op.mnemonic, "LD");

    get_opcode("0x24", op);
    ASSERT_EQ(op.mnemonic, "INC");

    get_opcode("0x25", op);
    ASSERT_EQ(op.mnemonic, "DEC");

    get_opcode("0x9E", op);
    ASSERT_EQ(op.mnemonic, "SBC");
}

TEST_F(DecoderTests, bytes)
{
    Opcode op;
    get_opcode("0x03", op);
    ASSERT_EQ(op.bytes, 1);

    get_opcode("0xCA", op);
    ASSERT_EQ(op.bytes, 3);

    get_opcode("0x06", op);
    ASSERT_EQ(op.bytes, 2);
}

TEST_F(DecoderTests, cycles)
{
    Opcode op;
    get_opcode("0x01", op);
    ASSERT_EQ(op.cycles[0], 12);

    get_opcode("0x07", op);
    ASSERT_EQ(op.cycles[0], 4);

    get_opcode("0x0A", op);
    ASSERT_EQ(op.cycles[0], 8);

    get_opcode("0x20", op);
    ASSERT_EQ(op.cycles[0], 12);
    ASSERT_EQ(op.cycles[1], 8);
}