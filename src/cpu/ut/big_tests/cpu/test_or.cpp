#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> constexpr or_opcodes{0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xF6};
} // namespace

TEST(test_cpu_BIG, test_all_or)
{
    for (auto hex : or_opcodes)
        validate_opcode(hex);
}