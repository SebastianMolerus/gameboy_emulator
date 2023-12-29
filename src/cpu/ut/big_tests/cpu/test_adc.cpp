#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> constexpr adc_opcodes{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0xCE};
} // namespace

TEST(test_cpu_BIG, test_all_adc)
{
    for (auto hex : adc_opcodes)
        validate_opcode(hex);
}