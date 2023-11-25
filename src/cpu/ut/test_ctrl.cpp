#include <chrono>
#include <cpu.hpp>
#include <gtest/gtest.h>
#include <translator.hpp>

TEST(Ctrl, ime_enabled_after_one_instruction)
{
    std::string assembly{R"(
        EI
        NOP
        NOP
        NOP
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].m_IME, false);
    ASSERT_EQ(expected_data[1].m_IME, true);
    ASSERT_EQ(expected_data[2].m_IME, true);
    ASSERT_EQ(expected_data[3].m_IME, true);
}

TEST(Ctrl, ime_disabled_after_one_instruction)
{
    std::string assembly{R"(
        EI
        NOP
        DI
        NOP
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 4);

    ASSERT_EQ(expected_data[0].m_IME, false);
    ASSERT_EQ(expected_data[1].m_IME, true);
    ASSERT_EQ(expected_data[2].m_IME, true);
    ASSERT_EQ(expected_data[3].m_IME, false);
}

TEST(Ctrl, trigger_vblank)
{
    std::string assembly{R"(
        LD SP, 0x1 
        LD [0xFFFF], SP // enable Vblank interrupt here ( IE )
        LD SP, 0xF      // Set stack at 0xF and check is PC is pushed after interrupt
        EI              // enable IME
        NOP             // PC is 0xB after executing this instruction
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    auto f = [](const CpuData &d, const Opcode &op) {
        // slow down execution to have vblank interrupt
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    };
    cpu.after_exec_callback(f);

    CpuData data;
    auto f2 = [&data](const CpuData &d) { data = d; };
    cpu.vblank_callback(f2);
    cpu.process();

    ASSERT_EQ(data.PC.u16, VBLANK_ADDR);        // next instruction is Vblank addr
    ASSERT_EQ(data.m_IME, false);               // ime was disabled by interruption
    ASSERT_EQ(data.SP.u16, 0xD);                // stack is decreased by two bytes
    ASSERT_EQ(data.m_memory[data.SP.u16], 0xB); // PC == 0xB was pushed to stack
}