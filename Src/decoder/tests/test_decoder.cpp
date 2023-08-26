#include <gtest/gtest.h>

#include <decoder.hpp>

#include <memory>

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