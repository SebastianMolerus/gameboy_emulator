#include "cpu_impl.hpp"

uint8_t cpu::cpu_impl::arith()
{
    switch (m_op.m_hex)
    {
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x87:
        return ADD_A_REG8();
    case 0x86:
        return ADD_A_IHLI();
    default:
        no_op_defined();
    }
    return 0;
}

uint8_t cpu::cpu_impl::ADD_A_REG8()
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

uint8_t cpu::cpu_impl::ADD_A_IHLI()
{
    reset_all_flags();

    assert(m_op.m_operands[0].m_name);
    assert(m_op.m_operands[1].m_name);

    uint8_t &dest = m_reg.get_byte(m_op.m_operands[0].m_name);
    uint8_t src = m_rw_device.read(m_reg.HL());

    if (is_carry(dest, src))
        set(flag::C);

    if (is_half_carry(dest, src))
        set(flag::H);

    dest += src;

    if (m_reg.get_byte(m_op.m_operands[0].m_name) == 0)
        set(flag::Z);

    return m_op.m_cycles[0];
}