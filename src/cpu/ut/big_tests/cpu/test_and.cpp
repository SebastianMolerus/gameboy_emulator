#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> const and_opcodes{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xE6};

} // namespace

TEST(test_cpu_BIG, test_all_and)
{
    for (auto hex : and_opcodes)
    {
        validate_opcode(hex);
    }
}