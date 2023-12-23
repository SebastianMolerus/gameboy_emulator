#include "../big_tests_utils.h"
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <span>

registers r;
opcode opc;

static bool cb(registers const &reg, opcode const &op, uint8_t)
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

    void set_flags(uint8_t flags)
    {
        m_cpu.set_flags(flags);
    }
};

std::filesystem::path alu_test_data{TEST_DATA_PATH};

TEST(BIG_ALU, cpu_alu)
{
    ASSERT_TRUE(std::filesystem::exists(alu_test_data));

    auto result = big_tests::read_big_data(alu_test_data / "adc.json");

    for (auto &data : result)
    {
        {
            // 0xCE, ADC A, n8
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCE, data.y};
            bus b{opcodes, startup};
            b.go();

            if (r.F() != data.result.flags)
            {
                volatile int a = 10;
            }

            std::stringstream diagnostic_message;
            diagnostic_message << "Diagnostic:"
                               << "\n"
                               << "Start values: " << (int)data.x << " " << (int)data.y << " " << (int)data.flags
                               << "\n";
            diagnostic_message << "Expected values: " << (int)data.result.value << " " << (int)data.result.flags
                               << "\n";
            diagnostic_message << "Current values: " << (int)r.A() << " " << (int)r.F() << "\n";
            ASSERT_EQ(r.A(), data.result.value) << diagnostic_message.str();
            ASSERT_EQ(r.F(), data.result.flags) << diagnostic_message.str();
        }
        // {
        //     // 0x80, ADD A, B
        //     registers startup;
        //     startup.A() = data.x;
        //     startup.B() = data.y;
        //     startup.F() = data.flags;

        //     std::array<uint8_t, 1> opcodes{0x80};
        //     bus b{opcodes, startup};
        //     b.go();

        //     ASSERT_EQ(r.A(), data.result.value);
        //     ASSERT_EQ(r.F(), data.result.flags);
        // }

        // {
        //     // 0x81, ADD A, C
        //     registers startup;
        //     startup.A() = data.x;
        //     startup.C() = data.y;
        //     startup.F() = data.flags;

        //     std::array<uint8_t, 1> opcodes{0x81};
        //     bus b{opcodes, startup};
        //     b.go();

        //     ASSERT_EQ(r.A(), data.result.value);
        //     ASSERT_EQ(r.F(), data.result.flags);
        // }

        // {
        //     // 0xC6, ADD A, n8
        //     registers startup;
        //     startup.A() = data.x;
        //     startup.F() = data.flags;

        //     std::array<uint8_t, 2> opcodes{0xC6, data.y};
        //     bus b{opcodes, startup};
        //     b.go();

        //     ASSERT_EQ(r.A(), data.result.value);
        //     ASSERT_EQ(r.F(), data.result.flags);
        // }
    }
}