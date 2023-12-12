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

// 0x28
TEST(test_jmp, JR_Z_e8)
{
    std::string assembly{R"(
        ADD A, B
        JR Z, 0x2
        JR Z, 0x2
        JR Z, 0x84
        LD A, 0x5
        LD A, 0x0
        LD B, 0x5
        ADD A, B
        JR Z, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(9, assembly);

    ASSERT_EQ(expected_data[0].A(), 0x0);

    ASSERT_EQ(expected_data[1].PC(), 5);
    ASSERT_EQ(expected_data[2].PC(), 3);
    ASSERT_EQ(expected_data[3].PC(), 7);
    ASSERT_EQ(expected_data[4].A(), 0x5);

    ASSERT_FALSE(expected_data[7].is_flag_set(flag::Z));

    // No jump in last instruction because
    // Z flag is NOT set
    ASSERT_EQ(expected_data[8].PC(), 0x10);

    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[3], 12);
    ASSERT_EQ(wait_cycles[8], 8);
}

// 0x38
TEST(test_jmp, JR_C_e8)
{
    std::string assembly{R"(
        LD A, 0xFF
        LD B, 0x80
        ADD A, B
        JR C, 0x2
        JR C, 0x2
        JR C, 0x84
        LD A, 0x5
        LD A, 0x5
        LD B, 0x5
        ADD A, B
        JR C, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(11, assembly);

    ASSERT_TRUE(expected_data[2].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[3].PC(), 9);
    ASSERT_EQ(expected_data[4].PC(), 7);
    ASSERT_EQ(expected_data[5].PC(), 11);
    ASSERT_EQ(expected_data[6].A(), 0x5);
    ASSERT_FALSE(expected_data[9].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[10].PC(), 0x14);
}

// 0x18
TEST(test_jmp, JR_e8)
{
    std::string assembly{R"(
        LD A, 0x99
        JR 0x2
        LD A, 0x3
        JR 0x84
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(4, assembly);

    ASSERT_EQ(expected_data[1].PC(), 0x6);

    ASSERT_EQ(expected_data[2].PC(), 0x4);
    ASSERT_EQ(expected_data[2].A(), 0x99);

    ASSERT_EQ(expected_data[3].PC(), 0x6);
    ASSERT_EQ(expected_data[3].A(), 0x3);
}

// 0xC2
TEST(test_jmp, JP_NZ_a16)
{
    std::string assembly{R"(
        JP NZ, 0x5
        LD B, 0x5
        JP NZ, 0xA
        LD D, 0xFF
        ADD A, B
        JP NZ, 0xFFFF
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(4, assembly);

    ASSERT_EQ(expected_data[1].B(), 0x0);
    ASSERT_EQ(expected_data[1].D(), 0x0);

    ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));

    // No jump because Z flag is set
    ASSERT_EQ(expected_data[3].PC(), 0xE);
    ASSERT_EQ(expected_data[3].D(), 0x0);

    ASSERT_EQ(wait_cycles[0], 16);
    ASSERT_EQ(wait_cycles[1], 16);
    ASSERT_EQ(wait_cycles[3], 12);
}