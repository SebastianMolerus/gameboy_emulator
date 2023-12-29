#include "../big_tests_utils.h"

TEST(test_alu_BIG, swap)
{
    validate_prefixed("swap.json", 0x30);
}