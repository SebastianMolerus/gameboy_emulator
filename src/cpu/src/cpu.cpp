#include "cpu.hpp"
#include <algorithm>
#include <cassert>
#include <common.hpp>
#include <functional>

namespace
{
void ld(Opcode const &op)
{
}

void adc(Opcode const &op)
{
}
} // namespace

Cpu::Cpu(std::span<uint8_t> program) : m_program(program)
{
    bool static result{load_opcodes()};
    assert(result);
}

bool Cpu::fetch_instruction(uint8_t &opcode_hex)
{
    if (m_registers.PC.u16 >= m_program.size())
        return false;

    opcode_hex = m_program[m_registers.PC.u16];
    return true;
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

void Cpu::process()
{
    uint8_t opcode_hex;
    while (fetch_instruction(opcode_hex))
    {
        Opcode const &op = get_opcode(opcode_hex);
        m_registers.PC.u16 += op.bytes;
        exec(op);
        std::invoke(m_callback, m_registers, op);
    }
}

void Cpu::register_function_callback(std::function<void(const CpuData &, const Opcode &)> callback)
{
    m_callback = callback;
}
