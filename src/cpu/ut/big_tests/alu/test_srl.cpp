#include "../big_tests_utils.h"

TEST(test_alu_BIG, srl)
{
    validate_prefixed("srl", 0x38);
}