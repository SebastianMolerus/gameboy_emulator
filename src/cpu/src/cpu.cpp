#include <cassert>
#include <cpu.hpp>
#include <decoder.hpp>

namespace
{
constexpr uint8_t PREFIXED_OPCODE{0xCB};

} // namespace

cpu::cpu(rw_device &rw_device) : m_rw_device{rw_device}, m_mnemonic_map{{"LD", &cpu::ld}}
{
}

void cpu::start()
{
    load_opcodes();
    while (1)
    {
        uint8_t opcode = get_next_byte();

        Opcode op{get_opcode(opcode)};
        if (opcode == PREFIXED_OPCODE)
        {
            opcode = get_next_byte();
            op = get_pref_opcode(opcode);
        }

        // assert(m_mnemonic_map.contains(op.mnemonic));

        auto func = m_mnemonic_map[op.mnemonic];
        std::invoke(func, this);
    }
}

uint8_t cpu::get_next_byte()
{
    return m_rw_device.read(m_regs.m_PC.u16++);
}
