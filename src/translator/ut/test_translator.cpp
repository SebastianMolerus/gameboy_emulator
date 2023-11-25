#include <gtest/gtest.h>
#include <translator.hpp>

TEST(translator_test, translate_instructions_ld_1)
{
    std::string st{R"(
        LD D, 0x74
        LD BC, 0x66
        LD HL, SP + 0x99
        LD DE, 0x41
        INC BC
    )"};

    auto machine_code = translate(st);

    ASSERT_EQ(machine_code.size(), 11);

    ASSERT_EQ(machine_code[0], 0x16);
    ASSERT_EQ(machine_code[1], 0x74);
    ASSERT_EQ(machine_code[2], 0x01);
    ASSERT_EQ(machine_code[3], 0x66);
    ASSERT_EQ(machine_code[4], 0x00);
    ASSERT_EQ(machine_code[5], 0xF8);
    ASSERT_EQ(machine_code[6], 0x99);
    ASSERT_EQ(machine_code[7], 0x11);
    ASSERT_EQ(machine_code[8], 0x41);
    ASSERT_EQ(machine_code[9], 0x00);
    ASSERT_EQ(machine_code[10], 0x03);
}

TEST(translator_test, translate_instructions_ld_2)
{
    std::string st{R"(
        LD SP, 0x3E
        LD HL, SP + 0x23
        LD SP, 0xFFFF
        LD HL, SP + 0x1
        LD SP, 0x5
        LD HL, SP + 0x81
    )"};

    auto machine_code = translate(st);

    ASSERT_EQ(machine_code.size(), 15);

    ASSERT_EQ(machine_code[0], 0x31);
    ASSERT_EQ(machine_code[1], 0x3E);
    ASSERT_EQ(machine_code[2], 0x00);
    ASSERT_EQ(machine_code[3], 0xF8);
    ASSERT_EQ(machine_code[4], 0x23);
    ASSERT_EQ(machine_code[5], 0x31);
    ASSERT_EQ(machine_code[6], 0xFF);
    ASSERT_EQ(machine_code[7], 0xFF);
    ASSERT_EQ(machine_code[8], 0xF8);
    ASSERT_EQ(machine_code[9], 0x01);
    ASSERT_EQ(machine_code[10], 0x31);
    ASSERT_EQ(machine_code[11], 0x5);
    ASSERT_EQ(machine_code[12], 0x0);
    ASSERT_EQ(machine_code[13], 0xF8);
    ASSERT_EQ(machine_code[14], 0x81);
}

TEST(translator_test, translate_instructions_ld_3)
{
    std::string st{R"(
        LD [0x100], SP
    )"};

    auto machine_code = translate(st);

    ASSERT_EQ(machine_code[0], 0x08);
    ASSERT_EQ(machine_code[1], 0x00);
    ASSERT_EQ(machine_code[2], 0x01);
}

TEST(translator_test, translate_instructions_control)
{
    std::string st{R"(
        NOP
        STOP 0x7F
        HALT
        DI
        PREFIX
        EI
    )"};

    auto machine_code = translate(st);

    ASSERT_EQ(machine_code.size(), 7);

    ASSERT_EQ(machine_code[0], 0x00);
    ASSERT_EQ(machine_code[1], 0x10);
    ASSERT_EQ(machine_code[2], 0x7F);
    ASSERT_EQ(machine_code[3], 0x76);
    ASSERT_EQ(machine_code[4], 0xF3);
    ASSERT_EQ(machine_code[5], 0xCB);
    ASSERT_EQ(machine_code[6], 0xFB);
}

TEST(translator_test, translate_instructions_ld_4)
{
    std::string st{R"(
        LD SP, 0x3E
        LD [0xFFFF], SP
    )"};

    auto machine_code = translate(st);

    ASSERT_EQ(machine_code.size(), 6);

    ASSERT_EQ(machine_code[0], 0x31);
    ASSERT_EQ(machine_code[1], 0x3E);
    ASSERT_EQ(machine_code[2], 0x00);
    ASSERT_EQ(machine_code[3], 0x08);
    ASSERT_EQ(machine_code[4], 0xFF);
    ASSERT_EQ(machine_code[5], 0xFF);
}