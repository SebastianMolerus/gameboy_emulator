#include "../big_tests_utils.h"

TEST(test_alu_BIG, rlc)
{
    validate_prefixed("rlc.json", 0x00);
}