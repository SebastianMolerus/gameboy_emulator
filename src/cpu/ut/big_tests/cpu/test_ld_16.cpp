#include "../big_tests_utils.h"

namespace
{

std::array<uint8_t, 15> const ld_16_opcodes{0x01, 0x11, 0x21, 0x31, 0xC1, 0xD1, 0xE1, 0xF1,
                                            0xC5, 0xD5, 0xE5, 0xF5, 0xF8, 0x08, 0xF9};

} // namespace

TEST(test_cpu_BIG, test_all_l16)
{
    for (auto hex : ld_16_opcodes)
    {
        validate_opcode(hex);
    }
}