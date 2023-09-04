#include "common.hpp"
#include "cpu.hpp"
#include "decoder.hpp"
#include <cassert>
#include <cstring>
#include <span>

namespace
{
bool is_carry_8(uint8_t a, uint8_t b, uint8_t carry_val)
{
    int16_t result = a + b + carry_val;
    int16_t carry = result ^ a ^ b;
    return carry & 0x100;
}

bool is_carry_16(uint16_t a, uint16_t b, uint8_t carry_val)
{
    int32_t result = a + b + carry_val;
    int32_t carry = result ^ a ^ b;
    return carry & 0x10000;
}

bool is_half_carry(uint8_t a, uint8_t b, uint8_t carry_val)
{
    int16_t result = a + b + carry_val;
    int16_t carry = result ^ a ^ b;
    return carry & 0x10;
}

void perform_add_8bit_operation(uint8_t a, uint8_t b, CpuData &cpu_data, uint8_t carry_val)
{
    cpu_data.unset_flag(CpuData::FLAG_N);
    uint8_t result = a + b + carry_val;

    cpu_data.unset_flag(CpuData::FLAG_C);
    cpu_data.unset_flag(CpuData::FLAG_Z);
    cpu_data.unset_flag(CpuData::FLAG_H);

    if (is_carry_8(a, b, carry_val))
        cpu_data.set_flag(CpuData::FLAG_C);

    if (is_half_carry(a, b, carry_val))
        cpu_data.set_flag(CpuData::FLAG_H);

    if (result == 0)
        cpu_data.set_flag(CpuData::FLAG_Z);

    cpu_data.AF.hi = result;
}

void perform_add_16bit_operation(uint16_t a, uint16_t b, CpuData &cpu_data, uint8_t carry_val)
{
    cpu_data.unset_flag(CpuData::FLAG_N);

    uint16_t result = a + b + carry_val;

    cpu_data.unset_flag(CpuData::FLAG_H);
    cpu_data.unset_flag(CpuData::FLAG_C);

    if (is_carry_16(a, b, carry_val))
        cpu_data.set_flag(CpuData::FLAG_C);

    if (is_half_carry(a, b, carry_val))
        cpu_data.set_flag(CpuData::FLAG_H);

    cpu_data.HL.u16 = result;
}

void perform_sub_8bit_operation(uint8_t a, uint8_t b, CpuData &cpu_data, uint8_t carry_val)
{
    cpu_data.set_flag(CpuData::FLAG_N);
    perform_add_8bit_operation(a, (~b), cpu_data, carry_val);

    if (cpu_data.is_flag_set(CpuData::FLAG_C))
        cpu_data.unset_flag(CpuData::FLAG_C);
    else
        cpu_data.set_flag(CpuData::FLAG_C);
}

} // namespace

namespace AddOperation
{

void register_8bit_to_acc(Opcode const &op, CpuData &cpu_data, bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    uint8_t reg_val = *cpu_data.get_byte(op.operands[1].name);
    perform_add_8bit_operation(acc, reg_val, cpu_data, carry_val);
}

void memory_byte_to_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program, bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    perform_add_8bit_operation(acc, program[1], cpu_data, carry_val);
}

void byte_from_HL_address_to_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program,
                                 bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    uint16_t address_from_hl = *cpu_data.get_word(op.operands[1].name);
    perform_add_8bit_operation(acc, program[address_from_hl], cpu_data, carry_val);
}

void register_16bit_to_HL(Opcode const &op, CpuData &cpu_data)
{
    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint16_t HL = *cpu_data.get_word(op.operands[0].name);
    uint16_t reg_val = *cpu_data.get_word(op.operands[1].name);
    perform_add_16bit_operation(HL, reg_val, cpu_data, 0);
}

void memory_byte_to_sp(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    cpu_data.unset_flag(CpuData::FLAG_Z);
    cpu_data.unset_flag(CpuData::FLAG_N);

    uint16_t SP = *cpu_data.get_word(op.operands[0].name);
    uint8_t value = program[1];
    int8_t sign = value & 0x80 ? -1 : 1;
    value &= 0x7F;

    uint16_t res_16bit = SP + (sign * value);
    uint8_t half_carry_val = (((SP & 0xF) + (value & 0xF)) & 0x10);

    cpu_data.SP.u16 += res_16bit;

    cpu_data.unset_flag(CpuData::FLAG_H);
    cpu_data.unset_flag(CpuData::FLAG_C);

    // Set Carry Flag
    if (res_16bit & 0x100)
        cpu_data.set_flag(CpuData::FLAG_C);

    // Sef Half Carry Flag
    if (half_carry_val == 0x10)
        cpu_data.set_flag(CpuData::FLAG_H);
}

} // namespace AddOperation

namespace SubOperation
{
void register_8bit_from_acc(Opcode const &op, CpuData &cpu_data, bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = !cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    uint8_t reg_val = *cpu_data.get_byte(op.operands[1].name);

    perform_sub_8bit_operation(acc, reg_val, cpu_data, carry_val);
}

void byte_from_HL_address_from_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program,
                                   bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = !cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    uint16_t address_from_hl = *cpu_data.get_word(op.operands[1].name);
    perform_sub_8bit_operation(acc, program[address_from_hl], cpu_data, carry_val);
}

void memory_byte_from_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program, bool is_carry_flag_included)
{
    uint8_t carry_val = 0;
    if (is_carry_flag_included)
        carry_val = !cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;

    assert(op.operands[0].name != nullptr);
    assert(op.operands[1].name != nullptr);

    uint8_t acc = *cpu_data.get_byte(op.operands[0].name);
    perform_sub_8bit_operation(acc, program[1], cpu_data, carry_val);
}

} // namespace SubOperation

void ADD_Instruction(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    constexpr bool carry_flag_not_needed = false;
    switch (op.hex)
    {
    case 0x09:
    case 0x19:
    case 0x29:
    case 0x39:
        AddOperation::register_16bit_to_HL(op, cpu_data);
        break;
    case 0x80 ... 0x85:
        AddOperation::register_8bit_to_acc(op, cpu_data, carry_flag_not_needed);
        break;
    case 0x86:
        AddOperation::byte_from_HL_address_to_acc(op, cpu_data, program, carry_flag_not_needed);
        break;
    case 0x87:
        AddOperation::register_8bit_to_acc(op, cpu_data, carry_flag_not_needed);
        break;
    case 0xC6:
        AddOperation::memory_byte_to_acc(op, cpu_data, program, carry_flag_not_needed);
        break;
    case 0xE8:
        AddOperation::memory_byte_to_sp(op, cpu_data, program);
    default:
        assert(false);
        break;
    }
}
void ADC_Instruction(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    constexpr bool carry_flag_included = true;
    switch (op.hex)
    {
    case 0x88 ... 0x8D:
        AddOperation::register_8bit_to_acc(op, cpu_data, carry_flag_included);
        break;
    case 0x8E:
        AddOperation::byte_from_HL_address_to_acc(op, cpu_data, program, carry_flag_included);
        break;
    case 0x8F:
        AddOperation::register_8bit_to_acc(op, cpu_data, carry_flag_included);
        break;
    case 0xCE:
        AddOperation::memory_byte_to_acc(op, cpu_data, program, carry_flag_included);
        break;
    default:
        assert(false);
        break;
    }
}

void SUB_Instruction(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    constexpr bool carry_flag_not_needed = false;
    switch (op.hex)
    {
    case 0x90 ... 0x95:
        SubOperation::register_8bit_from_acc(op, cpu_data, carry_flag_not_needed);
        break;
    case 0x96:
        SubOperation::byte_from_HL_address_from_acc(op, cpu_data, program, carry_flag_not_needed);
        break;
    case 0x97:
        SubOperation::register_8bit_from_acc(op, cpu_data, carry_flag_not_needed);
        break;
    case 0xD6:
        SubOperation::memory_byte_from_acc(op, cpu_data, program, carry_flag_not_needed);
        break;
    default:
        assert(false);
        break;
    }
}

void SBC_Instruction(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    constexpr bool carry_flag_included = true;
    switch (op.hex)
    {
    case 0x98 ... 0x9D:
        SubOperation::register_8bit_from_acc(op, cpu_data, carry_flag_included);
        break;
    case 0x9F:
        SubOperation::register_8bit_from_acc(op, cpu_data, carry_flag_included);
        break;
    case 0x9E:
        SubOperation::byte_from_HL_address_from_acc(op, cpu_data, program, carry_flag_included);
        break;
    case 0xDE:
        SubOperation::memory_byte_from_acc(op, cpu_data, program, carry_flag_included);
        break;
    default:
        assert(false);
        break;
    }
}

void arithmetic(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    if (strcmp(op.mnemonic, MNEMONICS_STR[1]) == 0)
        ADD_Instruction(op, cpu_data, program);
    else if (strcmp(op.mnemonic, MNEMONICS_STR[0]) == 0)
        ADD_Instruction(op, cpu_data, program);
    else if (strcmp(op.mnemonic, MNEMONICS_STR[43]) == 0)
        SUB_Instruction(op, cpu_data, program);
    else if (strcmp(op.mnemonic, MNEMONICS_STR[40]) == 0)
        SBC_Instruction(op, cpu_data, program);
    else
        assert(false);
}