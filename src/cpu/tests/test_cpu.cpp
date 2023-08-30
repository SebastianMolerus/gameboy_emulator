#include "cpu.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <gtest/gtest.h>

TEST(CpuTest, basic_instruction_fetch)
{
    // Fake one, just for test
    std::array<uint8_t, 1> adc_instruction = {0x88};
    cpu::process(adc_instruction);
    ASSERT_EQ(cpu::BC(), 0xAABB);
    cpu::reset();

    std::array<uint8_t, 1> ld_instruction = {0x16};
    cpu::process(ld_instruction);
    ASSERT_EQ(cpu::BC(), 0xCCDD);
}