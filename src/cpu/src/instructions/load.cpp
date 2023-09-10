#include "cpu.hpp"
#include "decoder.hpp"
#include <bitset>
#include <cassert>
#include <cstring>
#include <variant>

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
void ld_a16_SP(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    uint16_t addr = get_16nn_le(program);
    cpu_data.m_memory[addr] = cpu_data.SP.u16;
    cpu_data.m_memory[addr + 1] = cpu_data.SP.u16 >> 8;
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

std::variant<uint8_t, uint16_t> get_operand_value(Operand const &operand, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(operand.name);

    uint16_t val;
    switch (operand.bytes)
    {
    case 1:
        return program[1];
    case 2:
        val = program[1];
        val <<= 8;
        val = program[2];
        return val;
    case 0:
        break;
    }

    switch (strlen(operand.name))
    {
    case 1:
        return *cpu_data.get_byte(operand.name);
    case 2:
        return *cpu_data.get_word(operand.name);
    default:
        assert(false);
    }
}

void ld_A_reg(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr); // target
    assert(op.operands[1].name != nullptr); // source

    // Source
    std::variant<uint8_t, uint16_t> src_value = get_operand_value(op.operands[1], cpu_data, program);
    if (!op.operands[1].immediate)
    {
        if (src_value.index() == 0)
        {
            uint16_t addr = std::get<uint8_t>(src_value);
            if (op.hex == 0xF2)
                addr += 0xFF00;
            src_value = cpu_data.m_memory[std::get<uint8_t>(src_value)];
        }

        else
            src_value = cpu_data.m_memory[std::get<uint16_t>(src_value)];
    }

    uint8_t val{};
    if (!op.operands[1].immediate)
    {
        if (strlen(op.operands[1].name) == 3)
        {
            assert(op.bytes == 3);
            assert(op.hex == 0xFA);

            uint16_t addr = get_16nn_le(program);
            val = cpu_data.m_memory[addr];
        }

        if (strlen(op.operands[1].name) == 2)
        {
            auto OP2 = cpu_data.get_word(op.operands[1].name);
            val = cpu_data.m_memory[*OP2];

            if (op.operands[1].increment == 1)
                *OP2 += 1;
            if (op.operands[1].decrement == 1)
                *OP2 -= 1;
        }

        if (strlen(op.operands[1].name) == 1)
        {
            assert(op.bytes == 1);
            assert(op.hex == 0xF2);
            val = cpu_data.m_memory[cpu_data.BC.lo + 0xFF00];
        }
    }
    else
    {
        if (op.bytes == 2)
        {
            val = program[1];
        }
        else
            val = *cpu_data.get_byte(op.operands[1].name);
    }

    *cpu_data.get_byte(op.operands[0].name) = std::get<uint8_t>(src_value);
}

} // namespace

// Main "LD" entry
void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    switch (op.hex)
    {
    case 0x08: // copy SP value into a16
        ld_a16_SP(op, cpu_data, program);
        break;
    case 0xF8: // add n to SP and copy it to HL
        ld_hl_sp_n8(op, cpu_data, program);
        break;
    case 0xF9: // copy HL to SP
        ld_sp_hl(op, cpu_data, program);
        break;
    case 0x01: // load nn to BC
    case 0x11: // load nn to DE
    case 0x21: // load nn to HL
    case 0x31: // load nn to SP
        ld_reg_n16(op, cpu_data, program);
        break;
    case 0x78: // load B to A
    case 0x79: // load C to A
    case 0x7A: // load D to A
    case 0x7B: // load E to A
    case 0x7C: // load H to A
    case 0x7D: // load L to A
    case 0x7E: // load ( HL ) to A
    case 0x7F: // load A to A ( wtf ? )
    case 0x2A: // load ( HL + ) to A
    case 0x3A: // load ( HL - ) to A
    case 0xF2: // Put value at address $FF00 + register C into A.
    case 0x3E: // load n to A
    case 0xFA: // load (a16) to A
    case 0x0A: // load (BC) to A
    case 0x1A: // load (DE) to A
    case 0x06: // load n to B
    case 0x02: // LD [BC], A
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
    default:
        assert(false);
    }
}