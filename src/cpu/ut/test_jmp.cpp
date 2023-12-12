#include <gtest/gtest.h>
#include <translator.hpp>
#include <utils.h>

using result_type = std::pair<std::vector<registers>, std::vector<uint8_t>>;

result_type get_cpu_output(int instructions_cc, std::string const &assembly)
{
    result_type result;
    int cc{};

    auto opcodes = translate(assembly);
    rw_mock mock{opcodes};
    cpu cpu{mock, [&result, &cc, &instructions_cc](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                result.first.push_back(regs);
                result.second.push_back(wait_cycles);
                ++cc;
                if (cc == instructions_cc)
                    return true;

                return false;
            }};
    cpu.start();

    return result;
}

// 0x20
TEST(test_jmp, JR_NZ_e8)
{
    std::string assembly{R"(
        JR NZ, 0x0
        LD B, 0x0
        ADD A, B
        JR NZ, 0x15
    )"};

    auto [registers, wait_cycles] = get_cpu_output(4, assembly);

    ASSERT_EQ(registers[0].PC(), 2);
    ASSERT_EQ(registers[3].PC(), 7);

    ASSERT_EQ(wait_cycles[0], 12);
    ASSERT_EQ(wait_cycles[3], 8);
}