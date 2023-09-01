#include "common.hpp"
#include "cpu.hpp"
#include "decoder.hpp"
#include <cstring>
#include <span>

namespace
{
void perform_add_8bit_operation(uint8_t a, uint8_t b, CpuData &cpu_data)
{
    uint8_t carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;
    uint8_t half_carry_val = (((a & 0xF) + (b & 0xF)) & 0x10);

    uint16_t res_16bit = a + b + carry_val;
    cpu_data.AF.hi = res_16bit & 0x00FF;

    cpu_data.unset_flag(CpuData::FLAG_C);
    cpu_data.unset_flag(CpuData::FLAG_Z);
    cpu_data.unset_flag(CpuData::FLAG_H);

    // Set Carry Flag
    if (res_16bit & 0x100)
        cpu_data.set_flag(CpuData::FLAG_C);

    // Set Zero Flag
    if (res_16bit == 0)
        cpu_data.set_flag(CpuData::FLAG_Z);

    // Sef Half Carry Flag
    if (half_carry_val == 0x10)
        cpu_data.set_flag(CpuData::FLAG_H);
}

void perform_add_16bit_operation(uint16_t a, uint16_t b, CpuData &cpu_data)
{
    uint8_t carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;
    uint16_t half_carry_val = (((a & 0xFF) + (b & 0xFF)) & 0x1000);

    uint32_t res_32bit = a + b + carry_val;
    cpu_data.HL.u16 = res_32bit & 0xFFFF;

    cpu_data.unset_flag(CpuData::FLAG_N);
    cpu_data.unset_flag(CpuData::FLAG_H);
    cpu_data.unset_flag(CpuData::FLAG_C);

    // Set Carry Flag
    if (res_32bit & 0x10000)
        cpu_data.set_flag(CpuData::FLAG_C);

    // Sef Half Carry Flag
    if (half_carry_val == 0x1000)
        cpu_data.set_flag(CpuData::FLAG_H);
}

// ADD - Add Register to Accumulator
void add_8bit_register_to_acc(Opcode const &op, CpuData &cpu_data)
{
    uint8_t *operand_1 = cpu_data.get_byte(op.operands[0].name);
    uint8_t *operand_2 = cpu_data.get_byte(op.operands[1].name);
    perform_add_8bit_operation(*operand_1, *operand_2, cpu_data);
}

void add_memory_byte_to_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    uint8_t *operand_1 = cpu_data.get_byte(op.operands[0].name);
    perform_add_8bit_operation(*operand_1, program[1], cpu_data);
}

void add_byte_from_HL_address_to_acc(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    uint8_t *operand_1 = cpu_data.get_byte(op.operands[0].name);
    uint16_t *operand_2 = cpu_data.get_word(op.operands[1].name);
    perform_add_8bit_operation(*operand_1, program[*operand_2], cpu_data);
}

void add_16bit_register_to_HL(Opcode const &op, CpuData &cpu_data)
{
    uint16_t *operand_1 = cpu_data.get_word(op.operands[0].name);
    uint16_t *operand_2 = cpu_data.get_word(op.operands[1].name);
    perform_add_16bit_operation(*operand_1, *operand_2, cpu_data);
}

} // namespace

void arithmetic(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    // ADD
    if (strcmp(op.mnemonic, MNEMONICS_STR[1]) != 0)
    {
        if (((0x80 <= op.hex) && (op.hex <= 0x85)) || op.hex == 0x87)
        {
            add_8bit_register_to_acc(op, cpu_data);
        }
        else if (op.hex == 0x86)
        {
            add_byte_from_HL_address_to_acc(op, cpu_data, program);
        }
        else if (op.hex == 0xC6)
        {
            add_memory_byte_to_acc(op, cpu_data, program);
        }
        else if ((op.hex & 0x9) == 0x9)
        {
            add_16bit_register_to_HL(op, cpu_data);
        }
    }
}
