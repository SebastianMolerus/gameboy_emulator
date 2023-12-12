#include <cpu.hpp>

uint8_t cpu::arith()
{
    switch (m_op.m_hex)
    {
    case 0x80:
        return ADD_REG8_REG8();
    default:
        no_op_defined();
    }
    return 0;
}

uint8_t cpu::ADD_REG8_REG8()
{
    reset_all_flags();

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