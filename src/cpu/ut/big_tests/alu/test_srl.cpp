#include "../big_tests_utils.h"

TEST(test_alu_BIG, srl)
{
    validate_prefixed("srl.json", 0x38);
}