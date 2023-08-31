#include "cpu.hpp"
#include "decoder.hpp"
#include <cstring>

namespace
{
// ADD - Add Register or Memory to Accumulator
void add_8bit_instruction(Opcode const &op, CpuData &cpu_data)
{
    uint8_t carry_val = (cpu_data.AF.lo & (1 << 3)) ? 1 : 0;

    uint8_t *operand_1 = cpu_data.get_byte(op.operands[0].name);
    uint8_t *operand_2 = cpu_data.get_byte(op.operands[2].name);
    uint16_t res_16bit = *operand_1 + *operand_2 + carry_val;
    cpu_data.AF.hi = res_16bit & 0x00FF;

    // Todo Flag Update
    if (res_16bit & 0x100) // Set Carry Flag
        cpu_data.AF.lo |= (1 << 3);
    else if (res_16bit == 0) // Zero Flag
        cpu_data.AF.lo |= (1 << 6);
}

void add_16bit_instruction(Opcode const &op, CpuData &cpu_data)
{
}
} // namespace

void arithmetic_logic(Opcode const &op, CpuData &cpu_data)
{
    if (strcmp(op.mnemonic, "ADD") != 0)
    {
        if ((0x80 <= op.bytes) && (op.bytes <= 0x87))
            add_8bit_instruction(op, cpu_data);
    }
}
