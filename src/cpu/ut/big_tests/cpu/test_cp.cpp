#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> constexpr cp_opcodes{0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xFE};
} // namespace

TEST(test_cpu_BIG, test_all_cp)
{
    for (auto hex : cp_opcodes)
        validate_opcode(hex);
}