#include "../big_tests_utils.h"

namespace
{

std::array<uint8_t, 2> const opcodes{0x27, 0x37};

} // namespace

TEST(test_cpu_BIG, test_all_other_alu)
{
    for (auto op : opcodes)
        validate_opcode(op);
}