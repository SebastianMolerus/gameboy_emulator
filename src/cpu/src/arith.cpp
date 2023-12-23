#include "cpu_impl.hpp"

namespace
{

}

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
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8F:
        return ADC_A_REG8();
    case 0x86:
        return ADD_A_IHLI();
    case 0xC6:
        return ADD_A_n8();
    case 0xCE:
        return ADC_n8();
    default:
        no_op_defined();
    }
    return 0;
}

// Add src to dst, save to src and set proper flags
void cpu::cpu_impl::add(uint8_t &dst, uint16_t src)
{
    reset_all_flags();

    if (is_carry(dst, src))
        set(flag::C);

    if (is_half_carry(dst, src))
        set(flag::H);

    dst += src;

    assert(m_op.m_operands[0].m_name);
    if (m_reg.get_byte(m_op.m_operands[0].m_name) == 0)
        set(flag::Z);
}

uint8_t cpu::cpu_impl::ADD_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    add(m_reg.A(), m_reg.get_byte(m_op.m_operands[1].m_name));
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADD_A_IHLI()
{
    add(m_reg.A(), m_rw_device.read(m_reg.HL()));
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADD_A_n8()
{
    add(m_reg.A(), m_op.m_data[0]);
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADC_n8()
{
    uint8_t const val = m_reg.F() & flag::C ? 1 : 0;
    uint8_t n8 = m_op.m_data[0];

    reset_all_flags();

    if (is_carry(n8, val))
        set(flag::C);
    if (is_half_carry(n8, val))
        set(flag::H);
    n8 += val;

    uint8_t &dest = m_reg.A();
    if (is_carry(dest, n8))
        set(flag::C);
    if (is_half_carry(dest, n8))
        set(flag::H);
    dest += n8;

    if (m_reg.get_byte(m_op.m_operands[0].m_name) == 0)
        set(flag::Z);
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADC_A_REG8()
{
    uint8_t const val = m_reg.F() & flag::C ? 1 : 0;
    assert(m_op.m_operands[1].m_name);
    uint8_t reg8 = m_reg.get_byte(m_op.m_operands[1].m_name);

    reset_all_flags();

    if (is_carry(reg8, val))
        set(flag::C);
    if (is_half_carry(reg8, val))
        set(flag::H);
    reg8 += val;

    uint8_t &dest = m_reg.A();
    if (is_carry(dest, reg8))
        set(flag::C);
    if (is_half_carry(dest, reg8))
        set(flag::H);
    dest += reg8;

    if (m_reg.get_byte(m_op.m_operands[0].m_name) == 0)
        set(flag::Z);
    return m_op.m_cycles[0];
}