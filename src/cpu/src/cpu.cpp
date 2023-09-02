#include "cpu.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <common.hpp>
#include <functional>
#include <unordered_map>

extern void arithmetic(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program);
extern void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program);

// There is opcode ld in arithmetic_logic section :/ [26]-LD (PLEASE add me to load)

using mnemonic_func = std::pair<const char *, std::function<void(Opcode const &, CpuData &, std::span<uint8_t>)>>;
std::array<mnemonic_func, 13> instruction_set{
    std::make_pair(MNEMONICS_STR[0], arithmetic),  std::make_pair(MNEMONICS_STR[1], arithmetic),
    std::make_pair(MNEMONICS_STR[2], arithmetic),  std::make_pair(MNEMONICS_STR[5], arithmetic),
    std::make_pair(MNEMONICS_STR[6], arithmetic),  std::make_pair(MNEMONICS_STR[7], arithmetic),
    std::make_pair(MNEMONICS_STR[8], arithmetic),  std::make_pair(MNEMONICS_STR[23], arithmetic),
    std::make_pair(MNEMONICS_STR[29], arithmetic), std::make_pair(MNEMONICS_STR[40], arithmetic),
    std::make_pair(MNEMONICS_STR[43], arithmetic), std::make_pair(MNEMONICS_STR[44], arithmetic),
    std::make_pair(MNEMONICS_STR[26], load)};

CpuData::CpuData()
    : m_register_map_word{{OPERANDS_STR[9], &AF.u16},
                          {OPERANDS_STR[11], &BC.u16},
                          {OPERANDS_STR[14], &DE.u16},
                          {OPERANDS_STR[17], &HL.u16},
                          {OPERANDS_STR[21], &SP.u16}},
      m_register_map_byte{{OPERANDS_STR[8], &AF.hi},  {OPERANDS_STR[10], &BC.hi}, {OPERANDS_STR[12], &BC.lo},
                          {OPERANDS_STR[13], &DE.hi}, {OPERANDS_STR[15], &DE.lo}, {OPERANDS_STR[16], &HL.hi},
                          {OPERANDS_STR[18], &HL.lo}}
{
}

uint16_t *CpuData::get_word(const char *reg_name)
{
    assert(m_register_map_word.contains(reg_name));
    return m_register_map_word[reg_name];
}

uint8_t *CpuData::get_byte(const char *reg_name)
{
    assert(m_register_map_byte.contains(reg_name));
    return m_register_map_byte[reg_name];
}

bool CpuData::is_flag_set(Flags flag)
{
    return (0 != (AF.lo & flag));
}

void CpuData::set_flag(Flags flag)
{
    AF.lo |= flag;
}

void CpuData::unset_flag(Flags flag)
{
    AF.lo &= ~flag;
}

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

void Cpu::exec(Opcode const &op)
{
    auto result = std::find_if(instruction_set.cbegin(), instruction_set.cend(),
                               [&op](mnemonic_func const &item) { return item.first == op.mnemonic; });
    assert(result != instruction_set.cend());
    auto const oldPC = m_registers.PC.u16;
    m_registers.PC.u16 += op.bytes;
    std::invoke(result->second, op, m_registers, m_program.subspan(oldPC));
}

void Cpu::process()
{
    uint8_t opcode_hex;
    while (fetch_instruction(opcode_hex))
    {
        Opcode const &op = get_opcode(opcode_hex);
        exec(op);
        std::invoke(m_callback, m_registers, op);
    }
}

void Cpu::register_function_callback(std::function<void(const CpuData &, const Opcode &)> callback)
{
    m_callback = callback;
}
