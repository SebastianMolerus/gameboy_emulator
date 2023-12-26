#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 14> const add_opcodes{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86,
                                          0x87, 0xC6, 0xE8, 0x09, 0x19, 0x29, 0x39};

} // namespace

TEST(test_cpu_BIG, test_all_add)
{
    for (auto hex : add_opcodes)
    {
        validate_opcode(hex);
    }
}