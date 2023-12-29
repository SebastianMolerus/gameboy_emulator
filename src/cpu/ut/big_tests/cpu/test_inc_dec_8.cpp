#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 16> constexpr id8_opcodes{0x04, 0x14, 0x24, 0x34, 0x05, 0x15, 0x25, 0x35,
                                              0x0C, 0x1C, 0x2C, 0x3C, 0x0D, 0x1D, 0x2D, 0x3D};
} // namespace

TEST(test_cpu_BIG, test_all_inc_dec_8)
{
    for (auto hex : id8_opcodes)
        validate_opcode(hex);
}