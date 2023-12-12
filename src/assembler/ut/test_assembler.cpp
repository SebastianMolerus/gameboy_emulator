#include <assembler.hpp>
#include <gtest/gtest.h>

TEST(translator_test, translate_instructions_ld_1)
{
    std::string st{R"(
        LD D, 0x74
        LD BC, 0x66
        LD HL, SP + 0x99
        LD DE, 0x41
        INC BC
    )"};

    auto machine_code = transform(st);

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

    auto machine_code = transform(st);

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

    auto machine_code = transform(st);

    ASSERT_EQ(machine_code[0], 0x08);
    ASSERT_EQ(machine_code[1], 0x00);
    ASSERT_EQ(machine_code[2], 0x01);
}

TEST(translator_test, jumps_2B_p1)
{
    std::string st{R"(
        JR NZ, 0xAB
        JR NC, 0x12
    )"};

    auto machine_code = transform(st);

    ASSERT_EQ(machine_code.size(), 4);
    ASSERT_EQ(machine_code[0], 0x20);
    ASSERT_EQ(machine_code[1], 0xAB);
    ASSERT_EQ(machine_code[2], 0x30);
    ASSERT_EQ(machine_code[3], 0x12);
}

TEST(translator_test, jumps_2B_p2)
{
    std::string st{R"(
        JR 0x78
        JR Z, 0xA7
        JR C, 0x1
    )"};

    auto machine_code = transform(st);

    ASSERT_EQ(machine_code.size(), 6);
    ASSERT_EQ(machine_code[0], 0x18);
    ASSERT_EQ(machine_code[1], 0x78);

    ASSERT_EQ(machine_code[2], 0x28);
    ASSERT_EQ(machine_code[3], 0xA7);

    ASSERT_EQ(machine_code[4], 0x38);
    ASSERT_EQ(machine_code[5], 0x1);
}

TEST(translator_test, jumps_1B)
{
    std::string st{R"(
        RET NZ
        RET NC
        RST $00
        RST $10
        RST $20
        RST $30
        RET Z
        RET C
        RET
        RETI
        JP HL
        RST $08
        RST $18
        RST $28
        RST $38
    )"};

    auto machine_code = transform(st);

    ASSERT_EQ(machine_code.size(), 15);
    ASSERT_EQ(machine_code[0], 0xC0);
    ASSERT_EQ(machine_code[1], 0xD0);
    ASSERT_EQ(machine_code[2], 0xC7);
    ASSERT_EQ(machine_code[3], 0xD7);
    ASSERT_EQ(machine_code[4], 0xE7);
    ASSERT_EQ(machine_code[5], 0xF7);
    ASSERT_EQ(machine_code[6], 0xC8);
    ASSERT_EQ(machine_code[7], 0xD8);
    ASSERT_EQ(machine_code[8], 0xC9);
    ASSERT_EQ(machine_code[9], 0xD9);
    ASSERT_EQ(machine_code[10], 0xE9);
    ASSERT_EQ(machine_code[11], 0xCF);
    ASSERT_EQ(machine_code[12], 0xDF);
    ASSERT_EQ(machine_code[13], 0xEF);
    ASSERT_EQ(machine_code[14], 0xFF);
}

TEST(translator_test, jumps_3B)
{
    std::string st{R"(
        JP NZ, 0x76
        JP NC, 0x123
        JP 0x7896
        CALL NZ, 0x1
        CALL NC, 0x320
        JP Z, 0x0
        JP C, 0x123F
        CALL Z, 0xFFFF
        CALL C, 0xFFF
        CALL 0xFF
    )"};

    auto machine_code = transform(st);

    ASSERT_EQ(machine_code.size(), 30);

    ASSERT_EQ(machine_code[0], 0xC2);
    ASSERT_EQ(machine_code[1], 0x76);
    ASSERT_EQ(machine_code[2], 0x00);

    ASSERT_EQ(machine_code[3], 0xD2);
    ASSERT_EQ(machine_code[4], 0x23);
    ASSERT_EQ(machine_code[5], 0x01);

    ASSERT_EQ(machine_code[6], 0xC3);
    ASSERT_EQ(machine_code[7], 0x96);
    ASSERT_EQ(machine_code[8], 0x78);

    ASSERT_EQ(machine_code[9], 0xC4);
    ASSERT_EQ(machine_code[10], 0x1);
    ASSERT_EQ(machine_code[11], 0x0);

    ASSERT_EQ(machine_code[12], 0xD4);
    ASSERT_EQ(machine_code[13], 0x20);
    ASSERT_EQ(machine_code[14], 0x03);

    ASSERT_EQ(machine_code[15], 0xCA);
    ASSERT_EQ(machine_code[16], 0x0);
    ASSERT_EQ(machine_code[17], 0x0);

    ASSERT_EQ(machine_code[18], 0xDA);
    ASSERT_EQ(machine_code[19], 0x3F);
    ASSERT_EQ(machine_code[20], 0x12);

    ASSERT_EQ(machine_code[21], 0xCC);
    ASSERT_EQ(machine_code[22], 0xFF);
    ASSERT_EQ(machine_code[23], 0xFF);

    ASSERT_EQ(machine_code[24], 0xDC);
    ASSERT_EQ(machine_code[25], 0xFF);
    ASSERT_EQ(machine_code[26], 0xF);

    ASSERT_EQ(machine_code[27], 0xCD);
    ASSERT_EQ(machine_code[28], 0xFF);
    ASSERT_EQ(machine_code[29], 0x0);
}