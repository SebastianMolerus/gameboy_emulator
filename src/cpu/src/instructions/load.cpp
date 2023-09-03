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

void push(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr);

    // Do we have enought space
    assert(cpu_data.SP.u16 >= 2);

    // decrease stack first
    cpu_data.SP.u16 -= 2;

    uint16_t const source_REG = *cpu_data.get_word(op.operands[0].name);
    cpu_data.m_memory[cpu_data.SP.u16] = static_cast<uint8_t>(source_REG);
    cpu_data.m_memory[cpu_data.SP.u16 + 1] = source_REG >> 8;
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

void ld_A_reg(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr); // target ( A )
    assert(op.operands[1].name != nullptr); // source

    *cpu_data.get_byte(op.operands[0].name) = *cpu_data.get_byte(op.operands[1].name);
}

void pop(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(cpu_data.SP.u16 <= 0xFFFD);
    assert(op.operands[0].name != nullptr);

    auto target = cpu_data.get_word(op.operands[0].name);
    uint16_t val = cpu_data.m_memory[cpu_data.SP.u16 + 1]; // MSB
    val <<= 8;
    val |= cpu_data.m_memory[cpu_data.SP.u16]; // LSB

    *target = val;

    // increase SP after pop
    cpu_data.SP.u16 += 2;
}

} // namespace

// Main "LD" entry
void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    switch (op.hex)
    {
    case 0x08: // copy a16 to SP
        ld_SP_a16(op, cpu_data, program);
        break;
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
        ld_reg_n16(op, cpu_data, program);
        break;
    case 0x78: // load B to A
    case 0x79: // load C to A
    case 0x7A: // load D to A
    case 0x7B: // load E to A
    case 0x7C: // load H to A
    case 0x7D: // load L to A
        ld_A_reg(op, cpu_data, program);
        break;
    case 0xF1: // pop AF
    case 0xC1: // pop BC
    case 0xD1: // pop DE
    case 0xE1: // pop HL
        pop(op, cpu_data, program);
        break;
    case 0xC5: // push BC
    case 0xF5: // push AF
    case 0xD5: // push DE
    case 0xE5: // push HL
        push(op, cpu_data, program);
        break;
    case 0xF8: // add n8 to SP and copy it to HL
        ld_hl_sp_n8(op, cpu_data, program);
        break;
    case 0xF9: // copy SP to HL
        ld_sp_hl(op, cpu_data, program);
        break;
    default:
        break;
    }
}