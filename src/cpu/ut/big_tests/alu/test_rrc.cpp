#include "../big_tests_utils.h"

TEST(test_alu_BIG, rrc)
{
    validate_prefixed("rrc.json", 0x08);
}