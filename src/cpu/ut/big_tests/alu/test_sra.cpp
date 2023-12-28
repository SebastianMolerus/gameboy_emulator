#include "../big_tests_utils.h"

TEST(test_alu_BIG, sra)
{
    validate_prefixed("sra", 0x28);
}