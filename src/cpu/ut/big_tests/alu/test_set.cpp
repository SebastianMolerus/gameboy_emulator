#include "../big_tests_utils.h"
#include <unordered_map>

namespace
{
std::array<uint8_t, 8> constexpr bit_0_opcodes{0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7};
std::array<uint8_t, 8> constexpr bit_1_opcodes{0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
std::array<uint8_t, 8> constexpr bit_2_opcodes{0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7};
std::array<uint8_t, 8> constexpr bit_3_opcodes{0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF};
std::array<uint8_t, 8> constexpr bit_4_opcodes{0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7};
std::array<uint8_t, 8> constexpr bit_5_opcodes{0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF};
std::array<uint8_t, 8> constexpr bit_6_opcodes{0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7};
std::array<uint8_t, 8> constexpr bit_7_opcodes{0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};
std::unordered_map<uint8_t, std::array<uint8_t, 8>> m_mapper{{0, bit_0_opcodes}, {1, bit_1_opcodes}, {2, bit_2_opcodes},
                                                             {3, bit_3_opcodes}, {4, bit_4_opcodes}, {5, bit_5_opcodes},
                                                             {6, bit_6_opcodes}, {7, bit_7_opcodes}};

} // namespace

TEST(test_alu_BIG, set)
{
    auto const all_data = read_alu_data(test_data_dir / "alu_tests" / "v1" / "set.json");

    for (auto const &data : all_data)
    {
        auto const arr = m_mapper.at(data.y);
        {
            registers startup;
            startup.B() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[0]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.B(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.C() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[1]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.C(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.D() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[2]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.D(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.E() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[3]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.E(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.H() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[4]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.H(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.L() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[5]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.L(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.HL() = 0xFF;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[6]};
            bus b{opcodes, startup};
            b.m_ram[0xFF] = data.x;
            b.go();

            ASSERT_EQ(b.m_ram[0xFF], data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, arr[7]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }
    }
}