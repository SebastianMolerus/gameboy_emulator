#include "cpu.hpp"
#include <cstdlib>
#include <ctime>
#include <gtest/gtest.h>
#include <iostream>
#include <translator.hpp>

// 0x02
TEST(test_load_8bit, LD_IBCI_A)
{
    std::string assembly{R"(
        LD BC, 0x59
        LD A, 0x15
        LD [BC], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x59], 0x15);
}

// 0x12
TEST(test_load_8bit, LD_IDEI_A)
{
    std::string assembly{R"(
        LD DE, 0x789F
        LD A, 0xEA
        LD [DE], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x789F], 0xEA);
}

// 0x22
TEST(test_load_8bit, LD_IHLplusI_A)
{
    std::string assembly{R"(
        LD HL, 0x9998
        LD A, 0x61
        LD [HL+], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x9998], 0x61);
    ASSERT_EQ(expected_data.HL.u16, 0x9999);
}

// 0x32
TEST(test_load_8bit, LD_IHLminusI_A)
{
    std::string assembly{R"(
        LD HL, 0x5
        LD A, 0x51
        LD [HL-], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0x5], 0x51);
    ASSERT_EQ(expected_data.HL.u16, 0x4);
}

// 0xE0
TEST(test_load_8bit, LDH_Ia8I_A)
{
    std::string assembly{R"(
        LD A, 0x99
        LDH [0x15], A
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0xFF00 + 0x15], 0x99);
}

// 0xF0
TEST(test_load_8bit, LDH_A_Ia8I)
{
    std::string assembly{R"(
        LD SP, 0x26
        LD [0xFF87], SP
        LDH A, [0x87]
    )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x26);
}

// 0x06
TEST(test_load_8bit, LD_B_n8)
{
    std::string assembly{R"(
         LD B, 0x07
         LD B, 0x1
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 2);
    ASSERT_EQ(expected_data[0].BC.hi, 0x07);
    ASSERT_EQ(expected_data[1].BC.hi, 0x1);
}

// 0x16
TEST(test_load_8bit, LD_D_n8)
{
    std::string assembly{R"(
         LD D, 0xAA
         LD D, 0xB
         LD D, 0x0
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].DE.hi, 0xAA);
    ASSERT_EQ(expected_data[1].DE.hi, 0xB);
    ASSERT_EQ(expected_data[2].DE.hi, 0x0);
}

// 0x26
TEST(test_load_8bit, LD_H_n8)
{
    std::string assembly{R"(
         LD H, 0x12
         LD H, 0x59
         LD H, 0x4
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].HL.hi, 0x12);
    ASSERT_EQ(expected_data[1].HL.hi, 0x59);
    ASSERT_EQ(expected_data[2].HL.hi, 0x4);
}

// 0x36
TEST(test_load_8bit, LD_IHLI_n8)
{
    std::string assembly{R"(
         LD HL, 0xFF32
         LD [HL], 0x8
         LD HL, 0x784E
         LD [HL], 0xF6
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.m_memory[0xFF32], 0x8);
    ASSERT_EQ(expected_data.m_memory[0x784E], 0xF6);
}

// 0x0E
TEST(test_load_8bit, LD_C_n8)
{
    std::string assembly{R"(
         LD C, 0x1
         LD C, 0x01
         LD C, 0xF5
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].BC.lo, 0x1);
    ASSERT_EQ(expected_data[1].BC.lo, 0x01);
    ASSERT_EQ(expected_data[2].BC.lo, 0xF5);
}

// 0x1E
TEST(test_load_8bit, LD_E_n8)
{
    std::string assembly{R"(
         LD E, 0x87
         LD E, 0xA4
         LD E, 0x8D
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].DE.lo, 0x87);
    ASSERT_EQ(expected_data[1].DE.lo, 0xA4);
    ASSERT_EQ(expected_data[2].DE.lo, 0x8D);
}

// 0x2E
TEST(test_load_8bit, LD_L_n8)
{
    std::string assembly{R"(
         LD L, 0x72
         LD L, 0x71
         LD L, 0x0F
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].HL.lo, 0x72);
    ASSERT_EQ(expected_data[1].HL.lo, 0x71);
    ASSERT_EQ(expected_data[2].HL.lo, 0x0F);
}

// 0x3E
TEST(test_load_8bit, LD_A_n8)
{
    std::string assembly{R"(
         LD A, 0x0
         LD A, 0x2
         LD A, 0x3
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data.push_back(d); };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[0].AF.hi, 0x0);
    ASSERT_EQ(expected_data[1].AF.hi, 0x2);
    ASSERT_EQ(expected_data[2].AF.hi, 0x3);
}

// 0x0A
TEST(test_load_8bit, LD_A_IBCI)
{
    std::string assembly{R"(
         LD SP, 0x8510
         LD [0x667], SP
         LD BC, 0x667
         LD A, [BC]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x10);
    ASSERT_EQ(expected_data.m_memory[0x667], 0x10);
    ASSERT_EQ(expected_data.m_memory[0x667 + 1], 0x85);
}

// 0x1A
TEST(test_load_8bit, LD_A_IDEI)
{
    std::string assembly{R"(
         LD SP, 0xAFAB
         LD [0x0099], SP
         LD DE, 0x0099
         LD A, [DE]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0xAB);
    ASSERT_EQ(expected_data.m_memory[0x0099], 0xAB);
    ASSERT_EQ(expected_data.m_memory[0x0099 + 1], 0xAF);
}

// 0x2A
TEST(test_load_8bit, LD_A_IHLplusI)
{
    std::string assembly{R"(
         LD SP, 0x1234
         LD [0xFF01], SP
         LD HL, 0xFF01
         LD A, [HL+]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x34);
    ASSERT_EQ(expected_data.m_memory[0xFF01], 0x34);
    ASSERT_EQ(expected_data.m_memory[0xFF01 + 1], 0x12);
    ASSERT_EQ(expected_data.HL.u16, 0xFF01 + 1);
}

// 0x3A
TEST(test_load_8bit, LD_A_IHLminusI)
{
    std::string assembly{R"(
         LD SP, 0x1234
         LD [0xFF01], SP
         LD HL, 0xFF01
         LD A, [HL+]
         LD A, [HL-]
     )"};
    auto opcodes = translate(assembly);
    Cpu cpu{opcodes};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.after_exec_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.AF.hi, 0x12);
    ASSERT_EQ(expected_data.m_memory[0xFF01], 0x34);
    ASSERT_EQ(expected_data.m_memory[0xFF01 + 1], 0x12);
    ASSERT_EQ(expected_data.HL.u16, 0xFF01);
}

namespace
{

std::string hex_string(uint16_t value)
{
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(4) << std::hex << value;
    return ss.str();
}

std::vector<uint8_t> fill_source(std::string_view src, uint8_t value)
{
    std::string assembly{"LD "};
    assembly += src;
    assembly += ", ";
    assembly += hex_string(value);
    return translate(assembly);
}

std::vector<uint8_t> fill_destination(std::string_view dest, std::string_view src)
{
    std::string assembly{"LD "};
    assembly += dest;
    assembly += ", ";
    assembly += src;
    return translate(assembly);
}

} // namespace

// 0x40 - 0x7F, without 0x76 ( halt )
TEST(test_load_8bit, LD_REG8_REG8)
{
    srand((unsigned)time(0));
    for (auto &src : {"A", "B", "C", "D", "E", "H", "L"})
        for (auto &dst : {"A", "B", "C", "D", "E", "H", "L"})
        {
            auto const some_value = rand() % std::numeric_limits<uint8_t>::max();

            auto op1 = fill_source(src, some_value);
            auto op2 = fill_destination(dst, src);
            op1.insert(op1.end(), op2.begin(), op2.end());

            Cpu cpu{op1};

            CpuData expected_data;
            auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
            cpu.after_exec_callback(f);
            cpu.process();

            std::stringstream ss;
            ss << "Assembly\n";
            ss << "LD " << src << ", " << hex_string(some_value) << "\n";
            ss << "LD " << dst << ", " << src << "\n";

            ASSERT_EQ(*expected_data.get_byte(dst), *expected_data.get_byte(src)) << ss.str();
        }
}

namespace
{
std::vector<uint8_t> prepare_HL(uint16_t value)
{
    std::string assembly{"LD SP, "};
    assembly += hex_string(value);
    assembly += '\n';

    assembly += std::string{R"(
            LD [0xABCD], SP
            LD HL, 0xABCD
        )"};
    return translate(assembly);
}
} // namespace

// 0x40 - 0x7F, without 0x76 ( halt )
TEST(test_load_8bit, LD_REG8_IHLI)
{
    auto const src = "[HL]";
    for (auto &dst : {"A", "B", "C", "D", "E", "H", "L"})
    {
        auto const some_value = rand() % std::numeric_limits<uint16_t>::max();

        auto opcodes = prepare_HL(some_value);

        std::string assembly{"LD "};
        assembly += dst;
        assembly += ", ";
        assembly += src;

        auto cmd = translate(assembly);

        opcodes.insert(opcodes.end(), cmd.begin(), cmd.end());

        Cpu cpu{opcodes};

        CpuData expected_data;
        auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
        cpu.after_exec_callback(f);
        cpu.process();

        ASSERT_EQ(*expected_data.get_byte(dst), static_cast<uint8_t>(some_value)) << assembly;
    }
}

// 0x40 - 0x7F, without 0x76 ( halt )
TEST(test_load_8bit, LD_IHLI_REG8)
{
    auto const dst = "[HL]";
    for (auto &src : {"A", "B", "C", "D", "E", "H", "L"})
    {
        auto const some_value = rand() % std::numeric_limits<uint8_t>::max();

        std::string assembly{"LD HL, 0xAB0F\n"};
        assembly += "LD ";
        assembly += src;
        assembly += ", ";
        assembly += hex_string(some_value);
        assembly += "\n";
        assembly += "LD ";
        assembly += dst;
        assembly += ", ";
        assembly += src;
        auto cmd = translate(assembly);

        Cpu cpu{cmd};

        CpuData expected_data;
        auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
        cpu.after_exec_callback(f);
        cpu.process();

        ASSERT_EQ(*expected_data.get_byte(src), some_value) << assembly;

        uint16_t addr{0xAB0F};
        if (strcmp(src, "H") == 0)
        {
            addr &= 0x00FF;
            addr |= static_cast<uint16_t>(some_value << 8);
        }
        if (strcmp(src, "L") == 0)
        {
            addr &= 0xFF00;
            addr |= static_cast<uint16_t>(some_value);
        }
        ASSERT_EQ(expected_data.m_memory[addr], some_value) << assembly;
    }
}