#include "../big_tests_utils.h"

TEST(test_alu_BIG, sub_8_bit)
{
    auto const result = read_alu_data("sub.json");
    for (auto const &data : result)
    {
        {
            // 0x90, SUB A, B
            registers startup;
            startup.A() = data.x;
            startup.B() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x90};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x91, SUB A, C
            registers startup;
            startup.A() = data.x;
            startup.C() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x91};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x92, SUB A, D
            registers startup;
            startup.A() = data.x;
            startup.D() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x92};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x93, SUB A, E
            registers startup;
            startup.A() = data.x;
            startup.E() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x93};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x94, SUB A, H
            registers startup;
            startup.A() = data.x;
            startup.H() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x94};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x95, SUB A, L
            registers startup;
            startup.A() = data.x;
            startup.L() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x95};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x96, SUB A, [HL]
            registers startup;
            startup.A() = data.x;
            startup.HL() = 0x9;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x96};
            bus b{opcodes, startup};
            b.m_ram[0x9] = data.y;
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0xD6, SUB A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xD6, data.y};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }
    }
}