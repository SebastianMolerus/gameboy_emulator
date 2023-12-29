#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 9> constexpr sub_opcodes{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0xD6};
} // namespace

TEST(test_cpu_BIG, test_all_sub)
{
    for (auto hex : sub_opcodes)
        validate_opcode(hex);
}