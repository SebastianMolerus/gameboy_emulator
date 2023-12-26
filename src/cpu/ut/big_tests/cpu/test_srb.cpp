#include "../big_tests_utils.h"

namespace
{

std::array<uint8_t, 1> const srb_opcodes{0x07};

} // namespace

TEST(test_cpu_BIG, test_all_srb)
{
    for (auto hex : srb_opcodes)
    {
        validate_opcode(hex);
    }
}