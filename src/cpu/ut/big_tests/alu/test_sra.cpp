#include "../big_tests_utils.h"

TEST(test_alu_BIG, sra)
{
    validate_prefixed("sra.json", 0x28);
}