#include "../big_tests_utils.h"

namespace
{
const std::filesystem::path rlc_tests_path{test_data_dir / "alu_tests" / "v1" / "rlc.json"};
}

TEST(test_alu_BIG, rlc)
{
    auto const result = read_alu_data(rlc_tests_path);

    for (auto const &data : result)
    {
        {
            // 0x00, RLC B
            registers startup;
            startup.B() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x00};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.B(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x01, RLC C
            registers startup;
            startup.C() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x01};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.C(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x02, RLC D
            registers startup;
            startup.D() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x02};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.D(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x03, RLC E
            registers startup;
            startup.E() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x03};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.E(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x04, RLC H
            registers startup;
            startup.H() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x04};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.H(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x05, RLC L
            registers startup;
            startup.L() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x05};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.L(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x06, RLC [HL]
            registers startup;
            startup.HL() = 0x678;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x06};
            bus b{opcodes, startup};
            b.m_ram[0x678] = data.x;
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(b.m_ram[0x678], data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x07, RLC A
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x07};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
    }
}