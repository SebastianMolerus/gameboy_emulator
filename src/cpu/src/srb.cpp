#include "cpu_impl.hpp"
#include <bit>
#include <bitset>

namespace
{
// Circular left rotate
// data[7] ( pre rotated ) is new Carry
void rotate_l(cpu::cpu_impl &cpu, uint8_t &data)
{
    std::bitset<8> b{std::rotl(data, 1)};
    if (b.test(0))
        cpu.set(flag::C);
    data = b.to_ulong();
}

// Circular right rotate
// data[0] ( pre rotated ) is new Carry
void rotate_r(cpu::cpu_impl &cpu, uint8_t &data)
{
    std::bitset<8> b{std::rotr(data, 1)};
    if (b.test(7))
        cpu.set(flag::C);
    data = b.to_ulong();
}

} // namespace

uint8_t cpu::cpu_impl::srb()
{
    switch (m_op.m_hex)
    {
    case 0x07:
        RLCA();
        break;
    case 0x17:
        RLA();
        break;
    case 0x0F:
        RRCA();
        break;
    case 0x1F:
        RRA();
        break;
    default:
        no_op_defined("SRB.cpp");
    }
    return m_op.m_cycles[0];
}

void cpu::cpu_impl::RLCA()
{
    reset_all_flags();
    rotate_l(*this, m_reg.A());
}

// RLCA + carry is copied into A[0]
void cpu::cpu_impl::RLA()
{
    bool const C = m_reg.F() & flag::C;
    RLCA();
    if (C)
        m_reg.A() |= 0x1;
    else
        m_reg.A() &= 0xFE;
}

void cpu::cpu_impl::RRCA()
{
    reset_all_flags();
    rotate_r(*this, m_reg.A());
}

void cpu::cpu_impl::RRA()
{
    bool const C = m_reg.F() & flag::C;
    RRCA();
    if (C)
        m_reg.A() |= 0x80;
    else
        m_reg.A() &= 0x7F;
}

// ******************************************
//              PREFIXED PART
// ******************************************
uint8_t cpu::cpu_impl::pref_srb()
{
    switch (m_op.m_hex)
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x07:
        RLC_REG8();
        break;
    case 0x06:
        RLC_IHLI();
        break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x17:
        RL_REG8();
        break;
    case 0x16:
        RL_IHLI();
        break;
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0F:
        RRC_REG8();
        break;
    case 0x0E:
        RRC_IHLI();
        break;
    default:
        no_op_defined("SRB_pref.cpp");
    }
    return m_op.m_cycles[0];
}

void cpu::cpu_impl::RLC_REG8()
{
    reset_all_flags();
    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);
    rotate_l(*this, REG8);
    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::RLC_IHLI()
{
    reset_all_flags();
    uint8_t data{m_rw_device.read(m_reg.HL())};
    rotate_l(*this, data);
    if (data == 0)
        set(flag::Z);
    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::RRC_REG8()
{
    reset_all_flags();
    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);
    rotate_r(*this, REG8);
    if (REG8 == 0)
        set(flag::Z);
}
void cpu::cpu_impl::RRC_IHLI()
{
    reset_all_flags();
    uint8_t data{m_rw_device.read(m_reg.HL())};
    rotate_r(*this, data);
    if (data == 0)
        set(flag::Z);
    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::RL_REG8()
{
    bool const C = m_reg.F() & flag::C;
    reset_all_flags();

    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);

    if (REG8 & 0x80)
    {
        set(flag::C);
    }
    else
        reset(flag::C);

    std::bitset<8> b{std::rotl(REG8, 1)};
    REG8 = b.to_ulong();

    if (C)
        REG8 |= 0x1;
    else
        REG8 &= 0xFE;

    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::RL_IHLI()
{
    bool const C = m_reg.F() & flag::C;
    reset_all_flags();

    uint8_t data{m_rw_device.read(m_reg.HL())};

    if (data & 0x80)
        set(flag::C);
    else
        reset(flag::C);

    std::bitset<8> b{std::rotl(data, 1)};
    data = b.to_ulong();

    if (C)
        data |= 0x1;
    else
        data &= 0xFE;

    if (data == 0)
        set(flag::Z);

    m_rw_device.write(m_reg.HL(), data);
}
