#include "cpu.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <common.hpp>
#include <functional>
#include <unordered_map>

extern void arithmetic(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program);
extern void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program);
extern void ctrl(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program);

namespace
{

using mnemonic_func = std::pair<const char *, std::function<void(Opcode const &, CpuData &, std::span<uint8_t>)>>;
std::array<mnemonic_func, 19> const instruction_set{
    std::make_pair(MNEMONICS_STR[0], arithmetic),  std::make_pair(MNEMONICS_STR[1], arithmetic),
    std::make_pair(MNEMONICS_STR[2], arithmetic),  std::make_pair(MNEMONICS_STR[5], arithmetic),
    std::make_pair(MNEMONICS_STR[6], arithmetic),  std::make_pair(MNEMONICS_STR[7], arithmetic),
    std::make_pair(MNEMONICS_STR[8], arithmetic),  std::make_pair(MNEMONICS_STR[23], arithmetic),
    std::make_pair(MNEMONICS_STR[29], arithmetic), std::make_pair(MNEMONICS_STR[40], arithmetic),
    std::make_pair(MNEMONICS_STR[43], arithmetic), std::make_pair(MNEMONICS_STR[44], arithmetic),
    std::make_pair(MNEMONICS_STR[26], load),       std::make_pair(MNEMONICS_STR[27], load),
    std::make_pair(MNEMONICS_STR[32], load),       std::make_pair(MNEMONICS_STR[30], load),
    std::make_pair(MNEMONICS_STR[28], ctrl),       std::make_pair(MNEMONICS_STR[9], ctrl),
    std::make_pair(MNEMONICS_STR[10], ctrl)};

constexpr uint8_t VBLANK_FLAG = 0x1;

} // namespace

Cpu::Cpu(std::span<uint8_t> program, uint16_t vblank_addr) : m_program(program)
{
    bool result{load_opcodes()};
    assert(result);
}

void Cpu::vblank()
{
    m_data.push_PC();
    m_data.IF() &= (~VBLANK_FLAG);
    m_data.m_IME = false;
    m_data.PC.u16 = VBLANK_ADDR;
    std::invoke(m_vblank_cb, m_data);
}

void Cpu::interrupt_check()
{
    if (!m_data.m_IME)
        return;

    uint8_t const ieflag{m_data.IE()};
    uint8_t const iflag{m_data.IF()};

    if (iflag & VBLANK_FLAG & ieflag)
    {
        vblank();
    }
}

void Cpu::ime()
{
    bool const prev_ime = m_data.m_IME;

    if (m_data.m_ime_trans == IME_TRANS::ENABLE_AFTER_ONE_INSTRUCTION)
    {
        m_data.m_IME = true;
    }
    else if (m_data.m_ime_trans == IME_TRANS::DISABLE_AFTER_ONE_INSTRUCTION)
    {
        m_data.m_IME = false;
    }

    if (prev_ime != m_data.m_IME)
    {
        m_data.m_ime_trans = IME_TRANS::DONT_TOUCH;
    }
}

bool Cpu::fetch_instruction(uint8_t &opcode_hex)
{
    interrupt_check();

    if (m_data.PC.u16 >= m_program.size())
        return false;

    opcode_hex = m_program[m_data.PC.u16];
    return true;
}

void Cpu::exec(Opcode const &op)
{
    auto result = std::find_if(instruction_set.cbegin(), instruction_set.cend(),
                               [&op](mnemonic_func const &item) { return item.first == op.mnemonic; });
    assert(result != instruction_set.cend());
    auto const oldPC = m_data.PC.u16;
    m_data.PC.u16 += op.bytes;
    std::invoke(result->second, op, m_data, m_program.subspan(oldPC));
}

void Cpu::process()
{
    auto dt = std::chrono::system_clock::now();
    uint8_t opcode_hex;
    while (fetch_instruction(opcode_hex))
    {
        auto const now = std::chrono::system_clock::now();
        auto const dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - dt).count();

        if (dur >= 16.6)
        {
            dt = now;
            m_data.IF() |= VBLANK_FLAG;
        }

        Opcode op;

        if (opcode_hex == 0xCB)
        {
            m_data.PC.u16 += 1;
            bool const fetchResult = fetch_instruction(opcode_hex);
            assert(fetchResult);

            op = get_pref_opcode(opcode_hex);
        }
        else
        {
            op = get_opcode(opcode_hex);
        }

        ime();

        exec(op);

        std::invoke(m_callback, m_data, op);
    }
}

void Cpu::vblank_callback(interrupt_callback callback)
{
    m_vblank_cb = callback;
}

void Cpu::after_exec_callback(std::function<void(const CpuData &, const Opcode &)> callback)
{
    m_callback = callback;
}