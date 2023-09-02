#include "cpu.hpp"
#include "decoder.hpp"
#include <bitset>
#include <cassert>

namespace
{

// get uin16_t which is saved in Little Endian
// with assumptions:
// program[0] -> opcode hex
// program[1] -> LSB
// program[2] -> MSB
uint16_t get_16nn_le(std::span<uint8_t> program)
{
    uint16_t nn = program[2];
    nn <<= 8;
    nn |= program[1];
    return nn;
}

// 0x08 : Put Stack Pointer (SP) at address n
void ld_SP_a16(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[1].name != nullptr);
    *cpu_data.get_word(op.operands[1].name) = get_16nn_le(program);
}

// put n16 into REG
void ld_reg_n16(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr);
    *cpu_data.get_word(op.operands[0].name) = get_16nn_le(program);
}

// 0xC5 PUSH BC
void push_BC(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr);

    // Do we have enought space
    assert(cpu_data.SP.u16 >= 2);

    // decrease stack first
    cpu_data.SP.u16 -= 2;

    uint16_t const BC = *cpu_data.get_word(op.operands[0].name);
    cpu_data.m_memory[cpu_data.SP.u16] = static_cast<uint8_t>(BC);
    cpu_data.m_memory[cpu_data.SP.u16 + 1] = BC >> 8;
}

// 0xF8 : Put SP + n effective address into HL
void ld_hl_sp_n8(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    cpu_data.unset_flag(CpuData::FLAG_Z);
    cpu_data.unset_flag(CpuData::FLAG_N);

    assert(op.operands[0].name != nullptr);

    auto *HL = cpu_data.get_word(op.operands[0].name);
    auto SP = cpu_data.SP.u16;

    uint8_t val = program[1];
    bool minus = val & 0x80;
    val &= 0x7F;

    if (minus)
        SP -= val;
    else
    {
        uint8_t half_carry_check{static_cast<uint8_t>(SP)};
        half_carry_check &= 0xF;
        half_carry_check += (val & 0xF);
        if (half_carry_check & 0x10)
            cpu_data.set_flag(CpuData::FLAG_H);

        uint32_t carry_check{SP};
        carry_check += val;
        if (carry_check & 0x10000)
            cpu_data.set_flag(CpuData::FLAG_C);

        SP += val;
    }
    *HL = SP;
}

// 0xF9 : load HL to SP
void ld_sp_hl(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr); // SP
    assert(op.operands[1].name != nullptr); // HL
    *cpu_data.get_word(op.operands[0].name) = *cpu_data.get_word(op.operands[1].name);
}

} // namespace

// Main "LD" entry
void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    switch (op.hex)
    {
    case 0x08:
        ld_SP_a16(op, cpu_data, program);
        break;
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
        ld_reg_n16(op, cpu_data, program);
        break;
    case 0xC5:
        push_BC(op, cpu_data, program);
        break;
    case 0xF8:
        ld_hl_sp_n8(op, cpu_data, program);
        break;
    case 0xF9:
        ld_sp_hl(op, cpu_data, program);
        break;
    default:
        break;
    }
}