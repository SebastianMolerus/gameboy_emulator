#include "../big_tests_utils.h"

TEST(test_alu_BIG, sla)
{
    validate_prefixed("sla", 0x20);
}