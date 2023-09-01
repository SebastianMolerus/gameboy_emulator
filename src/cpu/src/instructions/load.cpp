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

void ld_reg_n16(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr);
    *cpu_data.get_word(op.operands[0].name) = get_16nn_le(program);
}

// Put SP + n effective address into HL
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
        half_carry_check &= 0x0F;
        half_carry_check += val;
        if (half_carry_check & 0xF0)
            cpu_data.set_flag(CpuData::FLAG_H);

        uint32_t carry_check{SP};
        carry_check += val;
        if (carry_check & 0xFFFF0000)
            cpu_data.set_flag(CpuData::FLAG_C);

        SP += val;
    }
    *HL = SP;
}

} // namespace

// Main "LD" entry
void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    switch (op.hex)
    {
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
        ld_reg_n16(op, cpu_data, program);
        break;
    case 0xF8:
        ld_hl_sp_n8(op, cpu_data, program);
        break;
    default:
        break;
    }
}