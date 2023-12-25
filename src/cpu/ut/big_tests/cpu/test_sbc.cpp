#include "../big_tests_utils.h"

namespace
{

std::array<uint8_t, 9> const sbc_opcodes{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xDE};

} // namespace

TEST(test_cpu_BIG, test_all_sbc)
{
    for (auto hex : sbc_opcodes)
    {
        validate_opcode(hex);
    }
}