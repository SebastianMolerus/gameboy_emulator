#include <assembler.hpp>
#include <gtest/gtest.h>
#include <utils.h>

namespace
{

std::string build_ADD_A_REG8(std::string reg)
{
    std::string result;
    result += "LD A, 0x0\n";
    result += "LD " + reg + ", 0x0\n";
    result += "ADD A, " + reg + "\n"; // 2. Zero flag is set

    result += "LD A, 0x5\n";
    result += "ADD A, " + reg + "\n"; // 4. Zero flag is reset

    result += "LD A, 0x8\n";
    result += "LD " + reg + ", 0x9\n";
    result += "ADD A, " + reg + "\n"; // 7. HC is set

    result += "LD A, 0x8\n";
    result += "LD " + reg + ", 0x7\n";
    result += "ADD A, " + reg + "\n"; // 10. HC is reset

    result += "LD A, 0xFF\n";
    result += "LD " + reg + ", 0x2\n";
    result += "ADD A, " + reg + "\n"; // 13. C is set

    result += "LD A, 0x80\n";
    result += "LD " + reg + ", 0x7F\n";
    result += "ADD A, " + reg + "\n"; // 16. C is reset

    return result;
}

#define ASSERT_Z(reg) ASSERT_TRUE(reg.is_flag_set(flag::Z));
#define ASSERT_N(reg) ASSERT_TRUE(reg.is_flag_set(flag::N));
#define ASSERT_H(reg) ASSERT_TRUE(reg.is_flag_set(flag::H));
#define ASSERT_C(reg) ASSERT_TRUE(reg.is_flag_set(flag::C));

#define ASSERT_not_Z(reg) ASSERT_FALSE(reg.is_flag_set(flag::Z));
#define ASSERT_not_N(reg) ASSERT_FALSE(reg.is_flag_set(flag::N));
#define ASSERT_not_H(reg) ASSERT_FALSE(reg.is_flag_set(flag::H));
#define ASSERT_not_C(reg) ASSERT_FALSE(reg.is_flag_set(flag::C));

} // namespace

// 0x80 81 82 83 84 85
TEST(test_arith, ADD_A_REG8)
{
    // Zero
    // Not zero
    // Half C
    // no half C
    // C
    // no C
    for (auto const reg8 : {"B", "C", "D", "E", "H", "L"})
    {
        rw_mock mock(build_ADD_A_REG8(reg8));
        auto [expected_data, wait_cycles] = mock.get_cpu_output();

        ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));
        ASSERT_FALSE(expected_data[2].is_flag_set(flag::N));
        ASSERT_EQ(expected_data[2].A(), 0);
        ASSERT_EQ(wait_cycles[2], 4);

        ASSERT_FALSE(expected_data[4].is_flag_set(flag::Z));
        ASSERT_FALSE(expected_data[4].is_flag_set(flag::N));
        ASSERT_NE(expected_data[4].A(), 0);
        ASSERT_EQ(wait_cycles[4], 4);

        ASSERT_TRUE(expected_data[7].is_flag_set(flag::H));
        ASSERT_FALSE(expected_data[7].is_flag_set(flag::N));
        ASSERT_EQ(expected_data[7].A(), 0x8 + 0x9);
        ASSERT_EQ(wait_cycles[7], 4);

        ASSERT_FALSE(expected_data[10].is_flag_set(flag::H));
        ASSERT_FALSE(expected_data[10].is_flag_set(flag::N));
        ASSERT_EQ(expected_data[10].A(), 0x8 + 0x7);
        ASSERT_EQ(wait_cycles[10], 4);

        ASSERT_TRUE(expected_data[13].is_flag_set(flag::C));
        ASSERT_FALSE(expected_data[13].is_flag_set(flag::N));
        ASSERT_EQ(expected_data[13].A(), 0x1);
        ASSERT_EQ(wait_cycles[13], 4);

        ASSERT_FALSE(expected_data[16].is_flag_set(flag::C));
        ASSERT_FALSE(expected_data[16].is_flag_set(flag::N));
        ASSERT_EQ(expected_data[16].A(), 0x80 + 0x7F);
        ASSERT_EQ(wait_cycles[16], 4);
    }
}

// 0x87
TEST(test_arith, ADD_A_A)
{
    // Zero
    // Not zero
    // Half C
    // no half C
    // C
    // no C
    rw_mock mock(R"(
        LD A, 0x0
        ADD A, A     ; 1. Z
        LD A, 0x4
        ADD A, A     ; 3. NZ
        LD A, 0x8
        ADD A, A     ; 5. HC
        LD A, 0x7     
        ADD A, A     ; 7. NHC
        LD A, 0x80
        ADD A, A     ; 9. C
        LD A, 0x67
        ADD A, A     ; 11. NC
    )");
    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_TRUE(expected_data[1].is_flag_set(flag::Z));
    ASSERT_FALSE(expected_data[1].is_flag_set(flag::N));
    ASSERT_EQ(expected_data[1].A(), 0);
    ASSERT_EQ(wait_cycles[1], 4);

    ASSERT_FALSE(expected_data[3].is_flag_set(flag::Z));
    ASSERT_FALSE(expected_data[3].is_flag_set(flag::N));
    ASSERT_NE(expected_data[3].A(), 0);
    ASSERT_EQ(wait_cycles[3], 4);

    ASSERT_TRUE(expected_data[5].is_flag_set(flag::H));
    ASSERT_FALSE(expected_data[5].is_flag_set(flag::N));
    ASSERT_EQ(expected_data[5].A(), 0x10);
    ASSERT_EQ(wait_cycles[5], 4);

    ASSERT_FALSE(expected_data[7].is_flag_set(flag::H));
    ASSERT_FALSE(expected_data[5].is_flag_set(flag::N));
    ASSERT_EQ(expected_data[7].A(), 0xE);
    ASSERT_EQ(wait_cycles[7], 4);

    ASSERT_TRUE(expected_data[9].is_flag_set(flag::C));
    ASSERT_FALSE(expected_data[9].is_flag_set(flag::N));
    ASSERT_EQ(expected_data[9].A(), 0x0);
    ASSERT_EQ(wait_cycles[9], 4);

    ASSERT_FALSE(expected_data[11].is_flag_set(flag::C));
    ASSERT_FALSE(expected_data[11].is_flag_set(flag::N));
    ASSERT_EQ(expected_data[11].A(), 0xCE);
    ASSERT_EQ(wait_cycles[11], 4);
}

// 0x86
TEST(test_arith, ADD_A_IHLI)
{
    // Zero
    // Not zero
    // Half C
    // no half C
    // C
    // no C
    rw_mock mock(R"(
        LD HL, 0xFEFE
        ADD A, [HL]     ; 1. Z 
        LD HL, 0xFEF0  
        ADD A, [HL]     ; 3. NZ
        LD A, 0x8 
        LD HL, 0x123
        ADD A, [HL]     ; 6. HC, A = 16
        LD A, 0x1
        LD HL, 0x123
        ADD A, [HL]     ; 9. NHC, A = 9
        LD A, 0x80
        LD HL, 0xF123
        ADD A, [HL]     ; 12. C, A = 0 
        LD A, 0x7F
        ADD A, [HL]     ; 14. NC, A = 255
    )");

    mock.m_ram[0xFEFE] = 0x0;
    mock.m_ram[0xFEF0] = 0x48;
    mock.m_ram[0x123] = 0x8;
    mock.m_ram[0xF123] = 0x80;

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_TRUE(expected_data[1].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[1].A(), 0x0);

    ASSERT_FALSE(expected_data[3].is_flag_set(flag::Z));
    ASSERT_NE(expected_data[3].A(), 0x0);

    ASSERT_TRUE(expected_data[6].is_flag_set(flag::H));
    ASSERT_EQ(expected_data[6].A(), 0x10);

    ASSERT_FALSE(expected_data[9].is_flag_set(flag::H));
    ASSERT_EQ(expected_data[9].A(), 0x9);

    ASSERT_TRUE(expected_data[12].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[12].A(), 0x0);

    ASSERT_FALSE(expected_data[14].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[14].A(), 0xFF);

    ASSERT_FALSE(expected_data[1].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[3].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[6].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[9].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[12].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[14].is_flag_set(flag::N));

    ASSERT_EQ(wait_cycles[1], 8);
    ASSERT_EQ(wait_cycles[3], 8);
    ASSERT_EQ(wait_cycles[6], 8);
    ASSERT_EQ(wait_cycles[9], 8);
    ASSERT_EQ(wait_cycles[12], 8);
    ASSERT_EQ(wait_cycles[14], 8);
}

// 0xC6
TEST(test_arith, ADD_A_n8)
{
    // Zero
    // Not zero
    // Half C
    // no half C
    // C
    // no C
    rw_mock mock(R"(
        LD A, 0x0
        ADD A, 0x0  ; 1. Z
        ADD A, 0x7E ; 2. NZ
        ADD A, 0x2  ; 3. H
        ADD A, 0xF  ; 4. NH
        ADD A, 0x71  ; 5. C
        ADD A, 0xFF  ; 6. NC
    )");
    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_TRUE(expected_data[1].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[1].A(), 0x0);

    ASSERT_FALSE(expected_data[2].is_flag_set(flag::Z));
    ASSERT_NE(expected_data[2].A(), 0x0);

    ASSERT_TRUE(expected_data[3].is_flag_set(flag::H));
    ASSERT_EQ(expected_data[3].A(), 0x80);

    ASSERT_FALSE(expected_data[4].is_flag_set(flag::H));
    ASSERT_EQ(expected_data[4].A(), 0x8F);

    ASSERT_TRUE(expected_data[5].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[5].A(), 0x0);

    ASSERT_FALSE(expected_data[6].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[6].A(), 0xFF);

    ASSERT_FALSE(expected_data[1].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[2].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[3].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[4].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[5].is_flag_set(flag::N));
    ASSERT_FALSE(expected_data[6].is_flag_set(flag::N));

    ASSERT_EQ(wait_cycles[1], 8);
    ASSERT_EQ(wait_cycles[2], 8);
    ASSERT_EQ(wait_cycles[3], 8);
    ASSERT_EQ(wait_cycles[4], 8);
    ASSERT_EQ(wait_cycles[5], 8);
    ASSERT_EQ(wait_cycles[6], 8);
}

// 0xCE
TEST(test_arith, ADC_n8)
{

    // rw_mock mock(R"(
    //     LD A, 0x80
    //     ADC A, 0x80  ; 1. C , A == 0
    //     LD A, 0x80
    //     ADC A, 0x7F  ; 3. C , A == 0
    //     LD A, 0x80
    //     ADC A, 0x80  ; 5. C  , A == 1
    //     ADC A, 0xE   ; 6, hC , na starcie A == 1,
    // )");
    // auto [expected_data, wait_cycles] = mock.get_cpu_output();

    // ASSERT_EQ(expected_data[1].A(), 0x0);
    // ASSERT_TRUE(expected_data[1].is_flag_set(flag::C));
    // ASSERT_TRUE(expected_data[1].is_flag_set(flag::Z));

    // ASSERT_EQ(expected_data[3].A(), 0x0);
    // ASSERT_TRUE(expected_data[3].is_flag_set(flag::C));
    // ASSERT_TRUE(expected_data[3].is_flag_set(flag::Z));

    // ASSERT_EQ(expected_data[5].A(), 0x1);
    // ASSERT_TRUE(expected_data[5].is_flag_set(flag::C));
    // ASSERT_FALSE(expected_data[5].is_flag_set(flag::Z));

    // ASSERT_EQ(expected_data[6].A(), 0x10);
    // ASSERT_FALSE(expected_data[6].is_flag_set(flag::Z));
    // ASSERT_TRUE(expected_data[6].is_flag_set(flag::H));

    // Zero
    // Not zero
    // Half C
    // no half C
    // C
    // no C

    rw_mock mock;
    {
        auto [regs, wait_cycles] = mock.reset_get_result_from(R"(
            LD A, 0x0
            ADC A, 0x0
        )");
        ASSERT_EQ(regs.A(), 0x0);
        ASSERT_Z(regs);
        ASSERT_not_C(regs);
        ASSERT_not_H(regs);
        ASSERT_not_N(regs);
    }

    {
        auto [regs, wait_cycles] = mock.reset_get_result_from(R"(
            ADC A, 0xF7
        )");
        ASSERT_EQ(regs.A(), 0xF7);
        ASSERT_not_Z(regs);
        ASSERT_not_C(regs);
        ASSERT_not_H(regs);
        ASSERT_not_N(regs);
    }

    {
        auto [regs, wait_cycles] = mock.reset_get_result_from(R"(
            LD A, 0x80
            ADC A, 0x80
        )");
        ASSERT_EQ(regs.A(), 0x00);
        ASSERT_Z(regs);
        ASSERT_C(regs);
        ASSERT_not_H(regs);
        ASSERT_not_N(regs);
    }

    {
        auto [regs, wait_cycles] = mock.reset_get_result_from(R"(
            LD A, 0x80
            ADC A, 0x80  ; C is Set, A == 0
            ADC A, 0xFF  ; C is Set, A == 0
        )");
        ASSERT_EQ(regs.A(), 0x00);
        ASSERT_Z(regs);
        ASSERT_C(regs);
        ASSERT_not_H(regs);
        ASSERT_not_N(regs);
    }

    {
        auto [regs, wait_cycles] = mock.reset_get_result_from(R"(
            LD A, 0x80
            ADC A, 0x80  ; C is Set, A == 0
            ADC A, 0xFE  ; A == 0xFF
        )");
        ASSERT_EQ(regs.A(), 0xFF);
        ASSERT_not_Z(regs);
        ASSERT_not_C(regs);
        ASSERT_not_H(regs);
        ASSERT_not_N(regs);
    }
}
