#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 8> constexpr id_opcodes{0x03, 0x13, 0x23, 0x33, 0x0B, 0x1B, 0x2B, 0x3B};
} // namespace

TEST(test_cpu_BIG, test_all_inc_dec_16)
{
    for (auto hex : id_opcodes)
        validate_opcode(hex);
}