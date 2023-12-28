#include "../big_tests_utils.h"

TEST(test_alu_BIG, swap)
{
    validate_prefixed("swap", 0x30);
}