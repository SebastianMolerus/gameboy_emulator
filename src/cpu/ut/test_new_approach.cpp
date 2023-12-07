#include "utils.h"
#include <gtest/gtest.h>
#include <translator.hpp>

TEST(test_load_16bit, LD_HL_SP_e8)
{
    std::string assembly{R"(
        LD SP, 0x3E
        LD HL, SP + 0x23
        LD SP, 0xFFFF
        LD HL, SP + 0x1
        LD SP, 0x5
        LD HL, SP + 0x81
    )"};
    auto opcodes = translate(assembly);
    rw_mock mock{6, opcodes};

    cpu c{mock};
    c.start();
}