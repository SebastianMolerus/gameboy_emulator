#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 4> constexpr srb_opcodes{0x07, 0x17, 0x0F, 0x1F};
} // namespace

TEST(test_cpu_BIG, test_all_srb)
{
    for (auto hex : srb_opcodes)
        validate_opcode(hex);
}