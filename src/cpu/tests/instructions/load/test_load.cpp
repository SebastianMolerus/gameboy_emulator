#include "cpu.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <span>
#include <vector>

TEST(LoadTest, ld_BC_n16)
{
    // 5831 == 0x16C7 -> C7, 16 in LE
    uint8_t a[] = {0x01, 0xC7, 0x16};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.BC.u16, 5831);
}

TEST(LoadTest, ld_DE_n16)
{
    // 5503 == 0x157F -> 7F, 15 in LE
    uint8_t a[] = {0x11, 0x7F, 0x15};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.DE.u16, 5503);
}

TEST(LoadTest, ld_HL_n16)
{
    // 62735 == 0xF50F -> 0F, F5 in LE
    uint8_t a[] = {0x21, 0x0F, 0xF5};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.HL.u16, 62735);
}

TEST(LoadTest, ld_SP_n16)
{
    // 43981 == 0xABCD -> CD, AB in LE
    uint8_t a[] = {0x31, 0xCD, 0xAB};
    Cpu cpu{a};
    CpuData expected_data;
    cpu.register_function_callback([&expected_data](const CpuData &d, const Opcode &) { expected_data = d; });
    cpu.process();
    ASSERT_EQ(expected_data.SP.u16, 43981);
}

TEST(LoadTest, ld_HL_SP_n8)
{
    // 1.
    // load 62 ( 0x3E ) to SP using ld_SP_n16
    // add 35 ( 0x23 ) to SP
    // HC expected

    // 2.
    // load 0xFFFF to SP
    // add 1 to SP
    // CF expected

    // 3.
    // load 0x05 to SP
    // substract 1 from SP ( 0x81 )

    uint8_t a[] = {0x31, 0x3E, 0x00, 0xF8, 0x23, 0x31, 0xFF, 0xFF, 0xF8, 0x01, 0x31, 0x05, 0x00, 0xF8, 0x81};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.size(), 3);
    ASSERT_EQ(expected_data[2].PC.u16, sizeof(a) / sizeof(uint8_t));

    for (auto &data : expected_data)
    {
        ASSERT_FALSE(data.is_flag_set(CpuData::FLAG_Z));
        ASSERT_FALSE(data.is_flag_set(CpuData::FLAG_N));
    }

    ASSERT_EQ(expected_data[0].HL.u16, 97);
    ASSERT_TRUE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));

    ASSERT_EQ(expected_data[1].HL.u16, 0);
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));

    ASSERT_EQ(expected_data[2].HL.u16, 4);
}

TEST(LoadTest, ld_HL_SP_n8_half_carry)
{
    // 1.
    // load 0x09 to SP
    // add 0x10 ( 16 ) to SP

    // 0000 1001
    //+0001 0000
    // No HC expected

    // 2.
    // load 0xA to SP
    // add 0xC to SP

    // 0000 1010
    //+0000 1100
    // HC expected

    uint8_t a[] = {0x31, 0x09, 0x00, 0xF8, 0x10, 0x31, 0x0A, 0x00, 0xF8, 0x0C};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_H));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_H));
}

TEST(LoadTest, ld_HL_SP_n8_carry)
{
    // 1.
    // load 0xFFFE to SP
    // add 0x01 to SP
    // No C expected

    // 2.
    // load 0xFFFF to SP
    // add 0x01 to SP
    // C expected

    uint8_t a[] = {0x31, 0xFE, 0xFF, 0xF8, 0x01, 0x31, 0xFF, 0xFF, 0xF8, 0x01};
    Cpu cpu{a};

    std::vector<CpuData> expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF8)
            expected_data.push_back(d);
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_FALSE(expected_data[0].is_flag_set(CpuData::FLAG_C));
    ASSERT_TRUE(expected_data[1].is_flag_set(CpuData::FLAG_C));
}

TEST(LoadTest, ld_SP_a16)
{
    // load 0xCDAB to SP

    uint8_t a[] = {0x08, 0xAB, 0xCD};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) { expected_data = d; };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.SP.u16, 0xCDAB);
}

TEST(LoadTest, ld_sp_hl)
{
    // fill HL
    // 0xABCD

    // copy HL to SP ( 0xF9 )
    uint8_t a[] = {0x21, 0xCD, 0xAB, 0xF9};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF9)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(expected_data.HL.u16, 0xABCD);
    ASSERT_EQ(expected_data.SP.u16, expected_data.HL.u16);
}

TEST(LoadTest, push_BC)
{
    // 1. set stack at 10
    // 2. load 0xFFAA into BC
    // 3. push BC ( 0xC5 )

    uint8_t a[] = {0x08, 0x0A, 0x00, 0x01, 0xAA, 0xFF, 0xC5};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xC5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 8);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0xAA);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0xFF);
}

TEST(LoadTest, push_DE)
{
    // 1. set stack at 0xBB
    // 2. load 0xAAFF into DE
    // 3. push DE ( 0xF5 )

    uint8_t a[] = {0x08, 0xBB, 0x00, 0x11, 0xFF, 0xAA, 0xD5};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xD5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xBB - 2);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0xFF);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0xAA);
}

TEST(LoadTest, push_HL)
{
    // 1. set stack at 0xAB
    // 2. load 0x1234 into HL
    // 3. push HL ( 0xE5 )

    uint8_t a[] = {0x08, 0xAB, 0x00, 0x21, 0x34, 0x12, 0xE5};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xE5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0xAB - 2);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0x34);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0x12);
}

TEST(LoadTest, push_AF)
{
    // 1. set stack at 0x0100
    // 2. set D as 0xBD
    // 3. load D to A
    // 4. push AF ( 0xF5 )

    uint8_t a[] = {0x08, 0x00, 0x01, 0x11, 0x00, 0xBD, 0x7A, 0xF5};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF5)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Is stack decremented by 2 bytes
    ASSERT_EQ(expected_data.SP.u16, 0x0100 - 2);

    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16], 0x0);
    ASSERT_EQ(expected_data.m_memory[expected_data.SP.u16 + 1], 0xBD);
}

TEST(LoadTest, pop_AF)
{
    // 1. set stack at 0xAB
    // 2. load 0x1234 into HL
    // 3. push HL ( 0xE5 )
    // 4. pop into AF ( 0xF1 )

    uint8_t a[] = {0x08, 0xAB, 0x00, 0x21, 0x34, 0x12, 0xE5, 0xF1};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xF1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Stack should be restored to original 0xAB
    ASSERT_EQ(expected_data.SP.u16, 0xAB);
    // AF should have same value as pushed HL
    ASSERT_EQ(expected_data.AF.u16, 0x1234);
}

TEST(LoadTest, pop_BC)
{
    // 1. set stack at 0xAB
    // 2. load 0x1234 into HL
    // 3. push HL ( 0xE5 )
    // 4. pop into BC ( 0xC1 )

    uint8_t a[] = {0x08, 0xAB, 0x00, 0x21, 0x34, 0x12, 0xE5, 0xC1};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xC1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Stack should be restored to original 0xAB
    ASSERT_EQ(expected_data.SP.u16, 0xAB);
    // BC should have same value as pushed HL
    ASSERT_EQ(expected_data.BC.u16, 0x1234);
}

TEST(LoadTest, pop_DE)
{
    // 1. set stack at 0xAB
    // 2. load 0x1234 into HL
    // 3. push HL ( 0xE5 )
    // 4. pop into DE ( 0xD1 )

    uint8_t a[] = {0x08, 0xAB, 0x00, 0x21, 0x34, 0x12, 0xE5, 0xD1};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xD1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Stack should be restored to original 0xAB
    ASSERT_EQ(expected_data.SP.u16, 0xAB);
    // DE should have same value as pushed HL
    ASSERT_EQ(expected_data.DE.u16, 0x1234);
}

TEST(LoadTest, pop_HL)
{
    // 1. set stack at 0xAB
    // 2. load 0x1234 into HL
    // 3. push HL ( 0xE5 )
    // 4. pop into HL ( 0xE1 )

    uint8_t a[] = {0x08, 0xAB, 0x00, 0x21, 0x34, 0x12, 0xE5, 0xE1};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0xE1)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    // Stack should be restored to original 0xAB
    ASSERT_EQ(expected_data.SP.u16, 0xAB);
    // HL should have same value as pushed HL
    ASSERT_EQ(expected_data.HL.u16, 0x1234);
}

TEST(LoadTest, load_D_to_A)
{
    // 1. set D as 0xCE
    // 2. load D to A ( 0x7A )

    uint8_t a[] = {0x11, 0x00, 0xCE, 0x7A};
    Cpu cpu{a};

    CpuData expected_data;
    auto f = [&expected_data](const CpuData &d, const Opcode &op) {
        if (op.hex == 0x7A)
            expected_data = d;
    };
    cpu.register_function_callback(f);
    cpu.process();

    ASSERT_EQ(*expected_data.get_byte("A"), 0xCE);
}
