#include "../big_tests_utils.h"

namespace
{
const std::filesystem::path rrc_tests_path{test_data_dir / "alu_tests" / "v1" / "rrc.json"};
}

TEST(test_alu_BIG, rrc)
{
    auto const result = read_alu_data(rrc_tests_path);

    for (auto const &data : result)
    {
        {
            // 0x08, RRC B
            registers startup;
            startup.B() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x08};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.B(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x09, RRC C
            registers startup;
            startup.C() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x09};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.C(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0A, RRC D
            registers startup;
            startup.D() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0A};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.D(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0B, RRC E
            registers startup;
            startup.E() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0B};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.E(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0C, RRC H
            registers startup;
            startup.H() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0C};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.H(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0D, RRC L
            registers startup;
            startup.L() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0D};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.L(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0E, RRC [HL]
            registers startup;
            startup.HL() = 0x678;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0E};
            bus b{opcodes, startup};
            b.m_ram[0x678] = data.x;
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(b.m_ram[0x678], data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x0F, RRC A
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, 0x0F};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
    }
}