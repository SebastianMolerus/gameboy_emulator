#include <assembler.hpp>
#include <gtest/gtest.h>
#include <utils.h>

// 0x20
TEST(test_jmp, JR_NZ_e8)
{
    std::string assembly{R"(
        JR NZ, 0x2
        JR NZ, 0x2
        JR NZ, 0x84
        LD A, 0x5
        LD A, 0x0
        LD B, 0x0
        ADD A, B
        JR NZ, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(8, assembly);

    // first instrution jumps to third
    ASSERT_EQ(expected_data[0].PC(), 4);

    // third instruction jumps to second
    // because 0x84 (e8) is treated like -4
    // so PC is decremented by 4
    ASSERT_EQ(expected_data[1].PC(), 2);

    // second instruction jumps to fourth
    ASSERT_EQ(expected_data[2].PC(), 6);

    ASSERT_EQ(expected_data[3].A(), 0x5);

    ASSERT_TRUE(expected_data[6].is_flag_set(flag::Z));

    // No jump in last instruction because
    // Z flag is set
    ASSERT_EQ(expected_data[7].PC(), 0xF);

    ASSERT_EQ(wait_cycles[0], 12);
    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[7], 8);
}

// 0x30
TEST(test_jmp, JR_NC_e8)
{
    std::string assembly{R"(
        JR NC, 0x2
        JR NC, 0x2
        JR NC, 0x84
        LD A, 0x5
        LD A, 0x80
        LD B, 0x80
        ADD A, B
        JR NC, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(8, assembly);

    ASSERT_EQ(expected_data[0].PC(), 4);
    ASSERT_EQ(expected_data[1].PC(), 2);
    ASSERT_EQ(expected_data[2].PC(), 6);
    ASSERT_EQ(expected_data[3].A(), 0x5);

    ASSERT_TRUE(expected_data[6].is_flag_set(flag::C));

    // No jump in last instruction because
    // C flag is set
    ASSERT_EQ(expected_data[7].PC(), 0xF);

    ASSERT_EQ(wait_cycles[0], 12);
    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[7], 8);
}