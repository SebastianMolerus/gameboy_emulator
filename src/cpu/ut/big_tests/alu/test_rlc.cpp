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
    }
}