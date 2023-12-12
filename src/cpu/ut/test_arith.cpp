#include <assembler.hpp>
#include <gtest/gtest.h>
#include <utils.h>

// 0x80
TEST(test_arith, ADD_A_B)
{
    std::string assembly{R"(
        LD B, 0x0
        ADD A, B
    )"};
    auto [expected_data, wait_cycles] = get_cpu_output(2, assembly);

    ASSERT_TRUE(expected_data[1].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[1].A(), 0x0);
    ASSERT_EQ(expected_data[1].B(), 0x0);
}

// 0x80
TEST(test_arith, ADD_A_B_v2)
{
    std::string assembly{R"(
        LD B, 0x1
        LD A, 0xF
        ADD A, B
    )"};
    auto [expected_data, wait_cycles] = get_cpu_output(3, assembly);

    ASSERT_FALSE(expected_data[2].is_flag_set(flag::Z));
    ASSERT_TRUE(expected_data[2].is_flag_set(flag::H));
    ASSERT_EQ(expected_data[2].A(), 0x10);
    ASSERT_EQ(expected_data[2].B(), 0x1);
    ASSERT_EQ(wait_cycles[2], 4);
}

// 0x80
TEST(test_arith, ADD_A_B_v3)
{
    std::string assembly{R"(
        LD B, 0x81
        LD A, 0x80
        ADD A, B
    )"};
    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};

    registers expected_data;
    cpu cpu{mock, [&expected_data](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == 0x80)
                {
                    expected_data = regs;
                    assert(wait_cycles == 4);
                    return true;
                }
                return false;
            }};
    cpu.start();

    // ASSERT_TRUE(expected_data.is_flag_set(flag::Z));
    ASSERT_FALSE(expected_data.is_flag_set(flag::H));
    ASSERT_TRUE(expected_data.is_flag_set(flag::C));
    ASSERT_EQ(expected_data.B(), 0x81);
    ASSERT_EQ(expected_data.A(), 0x01);
}