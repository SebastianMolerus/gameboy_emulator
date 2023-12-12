#include <cpu.hpp>
#include <sstream>

uint8_t cpu::arith()
{
    switch (m_op.m_hex)
    {
    case 0x80:
        return ADD_REG8_REG8();
    default:
        std::stringstream ss;
        ss << __func__ << ": no operation defined for opcode 0x" << std::hex << (int)m_op.m_hex;
        throw std::runtime_error(ss.str());
    }
    return 0;
}

uint8_t cpu::ADD_REG8_REG8()
{
    reset(flag::N);

    assert(m_op.m_operands[0].m_name);
    assert(m_op.m_operands[1].m_name);

    uint8_t &dest = m_reg.get_byte(m_op.m_operands[0].m_name);
    uint8_t src = m_reg.get_byte(m_op.m_operands[1].m_name);

    if (is_carry(dest, src))
        set(flag::C);

    if (is_half_carry(dest, src))
        set(flag::H);

    dest += src;

    if (m_reg.get_byte(m_op.m_operands[0].m_name) == 0)
        set(flag::Z);

    return m_op.m_cycles[0];
}