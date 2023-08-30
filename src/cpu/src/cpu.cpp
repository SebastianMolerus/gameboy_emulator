#include "cpu.hpp"
#include <common.hpp>
#include <decoder.hpp>

#include <algorithm>
#include <cassert>
#include <functional>

namespace
{

typedef union {
    uint16_t u16;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
} Register_u16;

struct
{
    Register_u16 AF{0x0000};
    Register_u16 BC{0x0000};
    Register_u16 DE{0x0000};
    Register_u16 HL{0x0000};
    Register_u16 SP{0x0000};
    Register_u16 PC{0x0000};

    uint32_t cycles{0};
} cpu_data;

bool fetch_instruction(std::span<uint8_t> program, uint8_t &opcode_hex)
{
    if (cpu_data.PC.u16 >= program.size())
        return false;

    opcode_hex = program[cpu_data.PC.u16];
    return true;
}

void ld(Opcode const &op)
{
    // fake one
    cpu_data.BC.u16 = 0xCCDD;
}

void adc(Opcode const &op)
{
    // fake one
    cpu_data.BC.u16 = 0xAABB;
}

using mnemonic_func = std::pair<const char *, std::function<void(Opcode const &)>>;
std::array<mnemonic_func, 2> instruction_set{std::make_pair(MNEMONICS_STR[0], adc),
                                             std::make_pair(MNEMONICS_STR[26], ld)};

void exec(Opcode const &op)
{
    auto result = std::find_if(instruction_set.cbegin(), instruction_set.cend(),
                               [&op](mnemonic_func const &item) { return item.first == op.mnemonic; });
    assert(result != instruction_set.cend());
    std::invoke(result->second, op);
}

} // namespace

namespace cpu
{

uint16_t AF()
{
    return cpu_data.AF.u16;
}
uint16_t BC()
{
    return cpu_data.BC.u16;
}
uint16_t DE()
{
    return cpu_data.DE.u16;
}
uint16_t HL()
{
    return cpu_data.HL.u16;
}
uint16_t SP()
{
    return cpu_data.SP.u16;
}
uint16_t PC()
{
    return cpu_data.PC.u16;
}

void process(std::span<uint8_t> program)
{
    bool static result{load_opcodes()};
    assert(result);
    uint8_t opcode_hex;
    while (fetch_instruction(program, opcode_hex))
    {
        Opcode const &op = get_opcode(opcode_hex);
        cpu_data.PC.u16 += op.bytes;
        exec(op);
    }
}

void reset()
{
    cpu_data.AF.u16 = cpu_data.BC.u16 = cpu_data.DE.u16 = cpu_data.HL.u16 = cpu_data.SP.u16 = cpu_data.PC.u16 = 0x0000;
    cpu_data.cycles = 0;
}

} // namespace cpu
