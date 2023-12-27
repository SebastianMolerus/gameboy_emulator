#include "../big_tests_utils.h"

namespace
{

std::array<uint8_t, 1> const rlc_opcodes{0xcb};

} // namespace

TEST(test_cpu_BIG, test_all_rcl)
{
    for (auto hex : rlc_opcodes)
        validate_opcode(hex);
}