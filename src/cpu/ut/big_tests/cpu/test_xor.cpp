#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> constexpr xor_opcodes{0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xEE};
} // namespace

TEST(test_cpu_BIG, test_all_xor)
{
    for (auto hex : xor_opcodes)
        validate_opcode(hex);
}