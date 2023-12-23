#include "../big_tests_utils.h"
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <span>

#define DIAGNOSTIC                                                                                                     \
    std::stringstream diagnostic_message;                                                                              \
    diagnostic_message << "Diagnostic:"                                                                                \
                       << "\n"                                                                                         \
                       << "Start values: " << (int)data.x << " " << (int)data.y << " " << (int)data.flags << "\n";     \
    diagnostic_message << "Expected values: " << (int)data.result.value << " " << (int)data.result.flags << "\n";      \
    diagnostic_message << "Current values: " << (int)r.A() << " " << (int)r.F() << "\n";

namespace
{
std::filesystem::path const alu_test_data{ALU_TEST_PATH};
registers r;
opcode opc;

bool cb(registers const &reg, opcode const &op, uint8_t)
{
    if (op.m_hex == 0)
        return true;
    r = reg;
    opc = op;
    return false;
}

struct bus : public rw_device
{
    std::array<uint8_t, 10> m_ram{};
    cpu m_cpu;

    bus(std::span<uint8_t> opcodes, registers startup_values) : m_cpu{*this, cb, startup_values}
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    virtual uint8_t read(uint16_t addr)
    {
        return m_ram[addr];
    }
    virtual void write(uint16_t addr, uint8_t data)
    {
        m_ram[addr] = data;
    }

    void go()
    {
        m_cpu.start();
    }
};

} // namespace

TEST(test_arith_BIG, adc_8_bit)
{
    ASSERT_TRUE(std::filesystem::exists(alu_test_data));

    auto const result = big_tests::read_big_data(alu_test_data / "adc.json");

    for (auto const &data : result)
    {
        {
            // 0xCE, ADC A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCE, data.y};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x88, ADC A, B
            registers startup;
            startup.A() = data.x;
            startup.B() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x88};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x89, ADC A, C
            registers startup;
            startup.A() = data.x;
            startup.C() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x89};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8A, ADC A, D
            registers startup;
            startup.A() = data.x;
            startup.D() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8A};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8B, ADC A, E
            registers startup;
            startup.A() = data.x;
            startup.E() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8B};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8C, ADC A, H
            registers startup;
            startup.A() = data.x;
            startup.H() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8C};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x8D, ADC A, L
            registers startup;
            startup.A() = data.x;
            startup.L() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0x8D};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
    }
}

TEST(test_arith_BIG, add_8_bit)
{
    ASSERT_TRUE(std::filesystem::exists(alu_test_data));

    auto const result = big_tests::read_big_data(alu_test_data / "add.json");

    for (auto const &data : result)
    {
        {
            // 0x80, ADD A, B
            registers startup;
            startup.A() = data.x;
            startup.B() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x80};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x81, ADD A, C
            registers startup;
            startup.A() = data.x;
            startup.C() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x81};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x82, ADD A, D
            registers startup;
            startup.A() = data.x;
            startup.D() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x82};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x83, ADD A, E
            registers startup;
            startup.A() = data.x;
            startup.E() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x83};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x84, ADD A, H
            registers startup;
            startup.A() = data.x;
            startup.H() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x84};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0x85, ADD A, L
            registers startup;
            startup.A() = data.x;
            startup.L() = data.y;
            startup.F() = data.flags;

            std::array<uint8_t, 1> opcodes{0x85};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }

        {
            // 0xC6, ADD A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xC6, data.y};
            bus b{opcodes, startup};
            b.go();

            DIAGNOSTIC
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
    }
}