#include "cpu_impl.hpp"

namespace
{

uint8_t sub(cpu::cpu_impl &cpu, uint8_t data)
{
    cpu.reset_all_flags();
    cpu.set(flag::N);

    if (data > cpu.m_reg.A())
        cpu.set(flag::C);

    if ((data & 0xf) > (cpu.m_reg.A() & 0xf))
        cpu.set(flag::H);

    cpu.m_reg.A() -= data;

    if (cpu.m_reg.A() == 0)
        cpu.set(flag::Z);
    return cpu.m_op.m_cycles[0];
}

void adc(cpu::cpu_impl &cpu, uint8_t &dst, uint8_t src)
{
    if (cpu.is_carry(dst, src))
        cpu.set(flag::C);
    if (cpu.is_half_carry(dst, src))
        cpu.set(flag::H);
    dst += src;
}

uint8_t adc_op(cpu::cpu_impl &cpu, uint8_t source, uint8_t carry_value)
{
    cpu.reset_all_flags();

    adc(cpu, source, carry_value);
    adc(cpu, cpu.m_reg.A(), source);

    if (cpu.m_reg.A() == 0)
        cpu.set(flag::Z);
    return cpu.m_op.m_cycles[0];
}

uint8_t add(cpu::cpu_impl &cpu, uint8_t &dst, uint16_t src)
{
    cpu.reset_all_flags();

    if (cpu.is_carry(dst, src))
        cpu.set(flag::C);

    if (cpu.is_half_carry(dst, src))
        cpu.set(flag::H);

    dst += src;

    assert(cpu.m_op.m_operands[0].m_name);
    if (cpu.m_reg.get_byte(cpu.m_op.m_operands[0].m_name) == 0)
        cpu.set(flag::Z);

    return cpu.m_op.m_cycles[0];
}

} // namespace

uint8_t cpu::cpu_impl::alu()
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
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x97:
        return SUB_A_REG8();
    case 0x96:
        return SUB_A_IHLI();
    case 0xD6:
        return SUB_A_n8();
    case 0x8E:
        return ADC_A_IHLI();
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

uint8_t cpu::cpu_impl::ADD_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    add(*this, m_reg.A(), m_reg.get_byte(m_op.m_operands[1].m_name));
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADD_A_IHLI()
{
    add(*this, m_reg.A(), m_rw_device.read(m_reg.HL()));
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADD_A_n8()
{
    add(*this, m_reg.A(), m_op.m_data[0]);
    return m_op.m_cycles[0];
}

uint8_t cpu::cpu_impl::ADC_n8()
{
    uint8_t const val = m_reg.F() & flag::C ? 1 : 0;
    uint8_t const n8 = m_op.m_data[0];
    return adc_op(*this, n8, val);
}

uint8_t cpu::cpu_impl::ADC_A_REG8()
{
    uint8_t const val = m_reg.F() & flag::C ? 1 : 0;
    assert(m_op.m_operands[1].m_name);
    uint8_t const reg8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    return adc_op(*this, reg8, val);
}

uint8_t cpu::cpu_impl::ADC_A_IHLI()
{
    uint8_t const val = m_reg.F() & flag::C ? 1 : 0;
    uint8_t const reg8 = m_rw_device.read(m_reg.HL());
    return adc_op(*this, reg8, val);
}

uint8_t cpu::cpu_impl::SUB_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const reg8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    return sub(*this, reg8);
}

uint8_t cpu::cpu_impl::SUB_A_IHLI()
{
    uint8_t const n8 = m_rw_device.read(m_reg.HL());
    return sub(*this, n8);
}

uint8_t cpu::cpu_impl::SUB_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    return sub(*this, n8);
}