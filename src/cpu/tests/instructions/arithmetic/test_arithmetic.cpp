#include "cpu.hpp"
#include <array>
#include <cstdint>
#include <gtest/gtest.h>

void add_8bit_reg_to_acc_operation_callback(const CpuData &cpu_data, const Opcode &op)
{
    static int n_calls = 0;
    // Ignore first opcode and set cpudata
    if (n_calls == 0)
    {
        CpuData *pCpuData = const_cast<CpuData *>(&cpu_data);
        pCpuData->BC.hi = 1;
        pCpuData->BC.lo = 1;
        pCpuData->DE.hi = 1;
        pCpuData->DE.lo = 1;
        pCpuData->HL.lo = 1;
        pCpuData->HL.hi = 1;
        pCpuData->PC.u16 = 1;
    }

    if (n_calls == 7)
    {
        // A = A + B = 1
        // A = A + C = 1 + 1 = 2
        // A = A + D = 2 + 1 = 3
        // A = A + E = 3 + 1 = 4
        // A = A + H = 4 + 1 = 5
        // A = A + L = 5 + 1 = 6
        // A = A + A = 6 + 6 = 12
        EXPECT_EQ(12, cpu_data.AF.hi);
        EXPECT_EQ(8, cpu_data.PC.u16);
    }
    ++n_calls;
}

void add_16bit_reg_to_hl_operation_callback(const CpuData &cpu_data, const Opcode &op)
{
    static int n_calls = 0;
    // Ignore first opcode and set cpudata
    if (n_calls == 0)
    {
        CpuData *pCpuData = const_cast<CpuData *>(&cpu_data);
        pCpuData->BC.u16 = 1;
        pCpuData->DE.u16 = 1;
        pCpuData->PC.u16 = 1;
        pCpuData->SP.u16 = 6;
    }

    if (n_calls == 4)
    {
        // HL = HL + BC = 1
        // HL = HL + DE = 2
        // HL = HL + HL = 4
        // HL = HL + SP = 4 + 6 = 10
        EXPECT_EQ(10, cpu_data.HL.u16);
        EXPECT_EQ(6, cpu_data.SP.u16);
        EXPECT_EQ(5, cpu_data.PC.u16);
    }
    ++n_calls;
}

TEST(ArithmeticTest, add_8bit_register_to_acc)
{
    // Ignore first opcode to prepare registers
    // Waiting for nop implementation
    std::array<uint8_t, 8> program = {0x80, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x87};
    Cpu cpu{program};
    cpu.register_function_callback(add_8bit_reg_to_acc_operation_callback);
    cpu.process();
}

TEST(ArithmeticTest, add_16bit_register_to_hl)
{
    // Ignore first opcode to prepare registers
    // Waiting for nop implementation
    std::array<uint8_t, 5> program = {0x09, 0x09, 0x19, 0x29, 0x39};
    Cpu cpu{program};
    cpu.register_function_callback(add_16bit_reg_to_hl_operation_callback);
    cpu.process();
}