#include "../big_tests_utils.h"
#include <unordered_map>

namespace
{
std::array<uint8_t, 8> constexpr bit_0_opcodes{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};
std::array<uint8_t, 8> constexpr bit_1_opcodes{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F};
std::array<uint8_t, 8> constexpr bit_2_opcodes{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97};
std::array<uint8_t, 8> constexpr bit_3_opcodes{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F};
std::array<uint8_t, 8> constexpr bit_4_opcodes{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7};
std::array<uint8_t, 8> constexpr bit_5_opcodes{0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF};
std::array<uint8_t, 8> constexpr bit_6_opcodes{0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7};
std::array<uint8_t, 8> constexpr bit_7_opcodes{0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF};
std::unordered_map<uint8_t, std::array<uint8_t, 8>> m_mapper{{0, bit_0_opcodes}, {1, bit_1_opcodes}, {2, bit_2_opcodes},
                                                             {3, bit_3_opcodes}, {4, bit_4_opcodes}, {5, bit_5_opcodes},
                                                             {6, bit_6_opcodes}, {7, bit_7_opcodes}};

} // namespace

TEST(test_alu_BIG, res)
{

    auto const all_data = read_alu_data("res.json");

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