#include "cpu_impl.hpp"

namespace
{

void sub(cpu::cpu_impl &cpu, uint8_t &dst, uint8_t src)
{
    if (cpu.is_carry_on_substraction_byte(dst, src))
        cpu.set(flag::C);
    if (cpu.is_half_carry_on_substraction_byte(dst, src))
        cpu.set(flag::H);
    dst -= src;
}

void add(cpu::cpu_impl &cpu, uint8_t &dst, uint8_t src)
{
    if (cpu.is_carry_on_addition_byte(dst, src))
        cpu.set(flag::C);
    if (cpu.is_half_carry_on_addition_byte(dst, src))
        cpu.set(flag::H);
    dst += src;
}

void sub_op(cpu::cpu_impl &cpu, uint8_t data)
{
    cpu.reset_all_flags();
    cpu.set(flag::N);

    sub(cpu, cpu.m_reg.A(), data);

    if (cpu.m_reg.A() == 0)
        cpu.set(flag::Z);
}

void adc_op(cpu::cpu_impl &cpu, uint8_t source)
{
    uint8_t const val = cpu.m_reg.F() & flag::C ? 1 : 0;
    cpu.reset_all_flags();

    add(cpu, source, val);
    add(cpu, cpu.m_reg.A(), source);

    if (cpu.m_reg.A() == 0)
        cpu.set(flag::Z);
}

void add_op(cpu::cpu_impl &cpu, uint8_t src)
{
    cpu.reset_all_flags();
    uint8_t &A = cpu.m_reg.A();

    add(cpu, A, src);

    if (A == 0)
        cpu.set(flag::Z);
}

void sbc_op(cpu::cpu_impl &cpu, uint8_t data)
{
    uint8_t const val = cpu.m_reg.F() & flag::C ? 1 : 0;
    cpu.reset_all_flags();
    cpu.set(flag::N);

    uint8_t &A = cpu.m_reg.A();

    add(cpu, data, val);
    sub(cpu, A, data);

    if (A == 0x0)
        cpu.set(flag::Z);
}

void and_op(cpu::cpu_impl &cpu, uint8_t data)
{
    cpu.reset_all_flags();
    cpu.set(flag::H);
    uint8_t &A = cpu.m_reg.A();
    A &= data;
    if (A == 0x0)
        cpu.set(flag::Z);
}

void xor_op(cpu::cpu_impl &cpu, uint8_t data)
{
    cpu.reset_all_flags();
    uint8_t &A = cpu.m_reg.A();
    A ^= data;
    if (A == 0x0)
        cpu.set(flag::Z);
}

void or_op(cpu::cpu_impl &cpu, uint8_t data)
{
    cpu.reset_all_flags();
    uint8_t &A = cpu.m_reg.A();
    A |= data;
    if (A == 0x0)
        cpu.set(flag::Z);
}

} // namespace

uint8_t cpu::cpu_impl::alu()
{
    switch (m_op.m_hex)
    {
    case 0x09:
    case 0x19:
    case 0x29:
    case 0x39:
        ADD_HL_REG16();
        break;
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x87:
        ADD_A_REG8();
        break;
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8F:
        ADC_A_REG8();
        break;
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x97:
        SUB_A_REG8();
        break;
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9F:
        SBC_A_REG8();
        break;
    case 0xA0:
    case 0xA1:
    case 0xA2:
    case 0xA3:
    case 0xA4:
    case 0xA5:
    case 0xA7:
        AND_A_REG8();
        break;
    case 0xA6:
        AND_A_IHLI();
        break;
    case 0xE6:
        AND_A_n8();
        break;
    case 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
    case 0xAC:
    case 0xAD:
    case 0xAF:
        XOR_A_REG8();
        break;
    case 0xAE:
        XOR_A_IHLI();
        break;
    case 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
    case 0xB4:
    case 0xB5:
    case 0xB7:
        OR_A_REG8();
        break;
    case 0xB6:
        OR_A_IHLI();
        break;
    case 0xF6:
        OR_A_n8();
        break;
    case 0xEE:
        XOR_A_n8();
        break;
    case 0xDE:
        SBC_A_n8();
        break;
    case 0x9E:
        SBC_A_IHLI();
        break;
    case 0x96:
        SUB_A_IHLI();
        break;
    case 0xD6:
        SUB_A_n8();
        break;
    case 0x8E:
        ADC_A_IHLI();
        break;
    case 0x86:
        ADD_A_IHLI();
        break;
    case 0xC6:
        ADD_A_n8();
        break;
    case 0xCE:
        ADC_n8();
        break;
    case 0xE8:
        ADD_SP_e8();
        break;
    default:
        no_op_defined();
    }
    return m_op.m_cycles[0];
}

void cpu::cpu_impl::ADD_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    add_op(*this, REG8);
}

void cpu::cpu_impl::ADD_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    add_op(*this, data);
}

void cpu::cpu_impl::ADD_A_n8()
{
    uint8_t const data = m_op.m_data[0];
    add_op(*this, data);
}

void cpu::cpu_impl::ADD_SP_e8()
{
    reset_all_flags();

    uint16_t &SP = m_reg.SP();
    uint8_t e8 = m_op.m_data[0];

    if (is_carry_on_addition_byte(SP, e8))
        set(flag::C);
    if (is_half_carry_on_addition_byte(SP, e8))
        set(flag::H);

    if (e8 & 0x80)
    {
        e8 -= 1;
        SP -= (uint8_t)(~e8);
    }
    else
        SP += e8;
}

void cpu::cpu_impl::ADD_HL_REG16()
{
    reset(flag::C);
    reset(flag::H);
    reset(flag::N);

    assert(m_op.m_operands[1].m_name);

    uint16_t REG16 = m_reg.get_word(m_op.m_operands[1].m_name);

    if (is_carry_on_addition_word(m_reg.HL(), REG16))
        set(flag::C);

    if (is_half_carry_on_addition_word(m_reg.HL(), REG16))
        set(flag::H);

    m_reg.HL() += REG16;
}

void cpu::cpu_impl::ADC_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    adc_op(*this, n8);
}

void cpu::cpu_impl::ADC_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    adc_op(*this, REG8);
}

void cpu::cpu_impl::ADC_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    adc_op(*this, data);
}

void cpu::cpu_impl::SUB_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const reg8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    sub_op(*this, reg8);
}

void cpu::cpu_impl::SUB_A_IHLI()
{
    uint8_t const n8 = m_rw_device.read(m_reg.HL());
    sub_op(*this, n8);
}

void cpu::cpu_impl::SUB_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    sub_op(*this, n8);
}

void cpu::cpu_impl::SBC_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    sbc_op(*this, REG8);
}

void cpu::cpu_impl::SBC_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    sbc_op(*this, data);
}

void cpu::cpu_impl::SBC_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    sbc_op(*this, n8);
}

void cpu::cpu_impl::AND_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    and_op(*this, REG8);
}

void cpu::cpu_impl::AND_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    and_op(*this, data);
}

void cpu::cpu_impl::AND_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    and_op(*this, n8);
}

void cpu::cpu_impl::XOR_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    xor_op(*this, REG8);
}
void cpu::cpu_impl::XOR_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    xor_op(*this, data);
}
void cpu::cpu_impl::XOR_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    xor_op(*this, n8);
}

void cpu::cpu_impl::OR_A_REG8()
{
    assert(m_op.m_operands[1].m_name);
    uint8_t const REG8 = m_reg.get_byte(m_op.m_operands[1].m_name);
    or_op(*this, REG8);
}
void cpu::cpu_impl::OR_A_IHLI()
{
    uint8_t const data = m_rw_device.read(m_reg.HL());
    or_op(*this, data);
}
void cpu::cpu_impl::OR_A_n8()
{
    uint8_t const n8 = m_op.m_data[0];
    or_op(*this, n8);
}