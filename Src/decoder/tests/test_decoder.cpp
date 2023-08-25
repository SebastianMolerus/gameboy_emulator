#include <gtest/gtest.h>

#include <decoder.hpp>

TEST(decoder_unit_tests, mnemonic_check)
{
    ASSERT_TRUE(load_opcodes());
    // https://gbdev.io/gb-opcodes/Opcodes.json

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