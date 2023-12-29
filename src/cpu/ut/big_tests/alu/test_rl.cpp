#include "../big_tests_utils.h"

TEST(test_alu_BIG, rl)
{
    validate_prefixed("rl.json", 0x10);
}