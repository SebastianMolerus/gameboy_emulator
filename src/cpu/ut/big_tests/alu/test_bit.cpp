#include "../big_tests_utils.h"
#include <unordered_map>

namespace
{
std::array<uint8_t, 8> constexpr bit_0_opcodes{0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
std::array<uint8_t, 8> constexpr bit_1_opcodes{0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F};
std::array<uint8_t, 8> constexpr bit_2_opcodes{0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57};
std::array<uint8_t, 8> constexpr bit_3_opcodes{0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F};
std::array<uint8_t, 8> constexpr bit_4_opcodes{0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67};
std::array<uint8_t, 8> constexpr bit_5_opcodes{0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F};
std::array<uint8_t, 8> constexpr bit_6_opcodes{0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77};
std::array<uint8_t, 8> constexpr bit_7_opcodes{0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};
std::unordered_map<uint8_t, std::array<uint8_t, 8>> m_mapper{{0, bit_0_opcodes}, {1, bit_1_opcodes}, {2, bit_2_opcodes},
                                                             {3, bit_3_opcodes}, {4, bit_4_opcodes}, {5, bit_5_opcodes},
                                                             {6, bit_6_opcodes}, {7, bit_7_opcodes}};

} // namespace

TEST(test_alu_BIG, bit)
{
    auto const all_data = read_alu_data("bit.json");
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