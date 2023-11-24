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

TEST(Ctrl, vblank)
{
    std::string assembly{R"(
        NOP
        NOP
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};
    cpu.enable_ir();

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        expected_data = d;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.PC.u16, VBLANK_ADDR);
}