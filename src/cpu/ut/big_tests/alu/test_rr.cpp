#include "../big_tests_utils.h"

TEST(test_alu_BIG, rr)
{
    validate_prefixed("rr", 0x18);
}