#include "../big_tests_utils.h"
#include <gtest/gtest.h>

TEST(test_alu_BIG, adc_8_bit)
{
    ASSERT_TRUE(std::filesystem::exists(alu_test_data));

    auto const result = read_big_data(alu_test_data / "adc.json");

    for (auto const &data : result)
    {
        {
            // 0xCE, ADC A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCE, data.y};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x88, ADC A, B
            registers startup;
            startup.A() = data.x;
            startup.B() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x88};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x89, ADC A, C
            registers startup;
            startup.A() = data.x;
            startup.C() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x89};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8A, ADC A, D
            registers startup;
            startup.A() = data.x;
            startup.D() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8A};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8B, ADC A, E
            registers startup;
            startup.A() = data.x;
            startup.E() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8B};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8C, ADC A, H
            registers startup;
            startup.A() = data.x;
            startup.H() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8C};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8D, ADC A, L
            registers startup;
            startup.A() = data.x;
            startup.L() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8D};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8E, ADC A, [HL]
            registers startup;
            startup.A() = data.x;
            startup.HL() = 0x9;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x8E};
            bus b{opcodes, startup};
            b.m_ram[0x9] = data.y;
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
    }
}
