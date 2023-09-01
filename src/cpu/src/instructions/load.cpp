#include "cpu.hpp"
#include "decoder.hpp"

namespace
{

// Load immediate to HL
// void ld_HL_n8(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
// {
//     uint16_t *HL = cpu_data.get_word(op.operands[0].name);
// }

} // namespace

void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    switch (op.hex)
    {
        // HL = n
    case 0x36:
        // ld_HL_n8(op, cpu_data);
    default:
        break;
    }
}