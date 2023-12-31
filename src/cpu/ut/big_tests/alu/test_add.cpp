#include "../big_tests_utils.h"

TEST(test_alu_BIG, add_8_bit)
{
    auto const result = read_alu_data("add.json");
    for (auto const &data : result)
    {
        {
            // 0x80, ADD A, B
            registers startup;
            startup.A() = data.x;
            startup.B() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x80};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x81, ADD A, C
            registers startup;
            startup.A() = data.x;
            startup.C() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x81};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x82, ADD A, D
            registers startup;
            startup.A() = data.x;
            startup.D() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x82};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x83, ADD A, E
            registers startup;
            startup.A() = data.x;
            startup.E() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x83};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x84, ADD A, H
            registers startup;
            startup.A() = data.x;
            startup.H() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x84};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x85, ADD A, L
            registers startup;
            startup.A() = data.x;
            startup.L() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x85};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0x86, ADD A, [HL]
            registers startup;
            startup.A() = data.x;
            startup.HL() = 0x9;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x86};
            bus b{opcodes, startup};
            b.m_ram[0x9] = data.y;
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            // 0xC6, ADD A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xC6, data.y};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }
    }
}