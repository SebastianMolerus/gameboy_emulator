#include "common.hpp"
#include "cpu.hpp"
#include "decoder.hpp"
#include <cstring>

namespace
{
void flag_update(const uint16_t operation_result, const uint8_t half_carry_result, CpuData &cpu_data)
{
    cpu_data.unset_flag(CpuData::FLAG_C);
    cpu_data.unset_flag(CpuData::FLAG_Z);
    cpu_data.unset_flag(CpuData::FLAG_H);

    // Set Carry Flag
    if (operation_result & 0x100)
        cpu_data.set_flag(CpuData::FLAG_C);

    // Set Zero Flag
    if (operation_result == 0)
        cpu_data.set_flag(CpuData::FLAG_Z);

    // Sef Half Carry Flag
    if (half_carry_result == 0x10)
        cpu_data.set_flag(CpuData::FLAG_H);
}

void perform_add_operation(uint8_t a, uint8_t b, CpuData &cpu_data)
{
    uint8_t carry_val = cpu_data.is_flag_set(CpuData::FLAG_C) ? 1 : 0;
    uint8_t half_carry_val = (((a & 0xF) + (b & 0xF)) & 0x10);

    uint16_t res_16bit = a + b + carry_val;
    cpu_data.AF.hi = res_16bit & 0x00FF;
    flag_update(res_16bit, half_carry_val, cpu_data);
}

// ADD - Add Register or Memory to Accumulator
void add_8bit_instruction(Opcode const &op, CpuData &cpu_data)
{
    uint8_t *operand_1 = cpu_data.get_byte(op.operands[0].name);
    uint8_t *operand_2 = cpu_data.get_byte(op.operands[2].name);
    perform_add_operation(*operand_1, *operand_2, cpu_data);
}
void addi_8bit_instruction(Opcode const &op, CpuData &cpu_data)
{
}

void add_16bit_instruction(Opcode const &op, CpuData &cpu_data)
{
}

} // namespace

void arithmetic_logic(Opcode const &op, CpuData &cpu_data)
{
    if (strcmp(op.mnemonic, MNEMONICS_STR[1]) != 0)
    {
        if ((0x80 <= op.bytes) && (op.bytes <= 0x87))
        {
            add_8bit_instruction(op, cpu_data);
        }
        else if (op.bytes == 0xC6) // This is adi instruction
        {
        }
    }
}
