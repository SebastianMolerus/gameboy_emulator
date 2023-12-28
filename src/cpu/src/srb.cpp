#include "cpu_impl.hpp"
#include <bit>
#include <bitset>

namespace
{
#define bitcheck(byte, nbit) ((byte) & (1 << (nbit)))
#define bitset(byte, nbit) ((byte) |= (1 << (nbit)))
#define bitclear(byte, nbit) ((byte) &= ~(1 << (nbit)))
// Circular left rotate
// data[7] ( pre rotated ) is new Carry
void rotate_c_l(cpu::cpu_impl &cpu, uint8_t &data)
{
    std::bitset<8> b{std::rotl(data, 1)};
    if (b.test(0))
        cpu.set(flag::C);
    data = b.to_ulong();
}

// Circular right rotate
// data[0] ( pre rotated ) is new Carry
void rotate_c_r(cpu::cpu_impl &cpu, uint8_t &data)
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
    rotate_c_l(*this, m_reg.A());
}

// RLCA + carry is copied into A[0]
void cpu::cpu_impl::RLA()
{
    bool const C = m_reg.F() & flag::C;
    RLCA();
    if (C)
        bitset(m_reg.A(), 0);
    else
        bitclear(m_reg.A(), 0);
}

void cpu::cpu_impl::RRCA()
{
    reset_all_flags();
    rotate_c_r(*this, m_reg.A());
}

void cpu::cpu_impl::RRA()
{
    bool const C = m_reg.F() & flag::C;
    RRCA();
    if (C)
        bitset(m_reg.A(), 7);
    else
        bitclear(m_reg.A(), 7);
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
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x27:
        SLA_REG8();
        break;
    case 0x26:
        SLA_IHLI();
        break;
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2F:
        SRA_REG8();
        break;
    case 0x2E:
        SRA_IHLI();
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
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1F:
        RR_REG8();
        break;
    case 0x1E:
        RR_IHLI();
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
    rotate_c_l(*this, REG8);
    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::RLC_IHLI()
{
    reset_all_flags();
    uint8_t data{m_rw_device.read(m_reg.HL())};
    rotate_c_l(*this, data);
    if (data == 0)
        set(flag::Z);
    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::RRC_REG8()
{
    reset_all_flags();
    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);
    rotate_c_r(*this, REG8);
    if (REG8 == 0)
        set(flag::Z);
}
void cpu::cpu_impl::RRC_IHLI()
{
    reset_all_flags();
    uint8_t data{m_rw_device.read(m_reg.HL())};
    rotate_c_r(*this, data);
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

    if (bitcheck(REG8, 7))
        set(flag::C);

    REG8 <<= 1;

    if (C)
        bitset(REG8, 0);
    else
        bitclear(REG8, 0);

    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::RL_IHLI()
{
    bool const C = m_reg.F() & flag::C;
    reset_all_flags();

    uint8_t data{m_rw_device.read(m_reg.HL())};

    if (bitcheck(data, 7))
        set(flag::C);

    data <<= 1;

    if (C)
        bitset(data, 0);
    else
        bitclear(data, 0);

    if (data == 0)
        set(flag::Z);

    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::RR_REG8()
{
    bool const C = m_reg.F() & flag::C;
    reset_all_flags();

    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);

    if (bitcheck(REG8, 0))
        set(flag::C);

    REG8 >>= 1;

    if (C)
        bitset(REG8, 7);
    else
        bitclear(REG8, 7);

    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::RR_IHLI()
{
    bool const C = m_reg.F() & flag::C;
    reset_all_flags();

    uint8_t data{m_rw_device.read(m_reg.HL())};

    if (bitcheck(data, 0))
        set(flag::C);

    data >>= 1;

    if (C)
        bitset(data, 7);
    else
        bitclear(data, 7);

    if (data == 0)
        set(flag::Z);

    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::SLA_REG8()
{
    reset_all_flags();

    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);

    if (bitcheck(REG8, 7))
        set(flag::C);

    REG8 <<= 1;

    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::SLA_IHLI()
{
    reset_all_flags();

    uint8_t data{m_rw_device.read(m_reg.HL())};

    if (bitcheck(data, 7))
        set(flag::C);

    data <<= 1;

    if (data == 0)
        set(flag::Z);

    m_rw_device.write(m_reg.HL(), data);
}

void cpu::cpu_impl::SRA_REG8()
{
    reset_all_flags();

    assert(m_op.m_operands[0].m_name);
    uint8_t &REG8 = m_reg.get_byte(m_op.m_operands[0].m_name);

    if (bitcheck(REG8, 0))
        set(flag::C);

    uint8_t const temp{REG8};
    REG8 >>= 1;
    REG8 |= temp & 0x80;

    if (REG8 == 0)
        set(flag::Z);
}

void cpu::cpu_impl::SRA_IHLI()
{
    reset_all_flags();

    uint8_t data{m_rw_device.read(m_reg.HL())};

    if (bitcheck(data, 0))
        set(flag::C);

    uint8_t const temp{data};
    data >>= 1;
    data |= temp & 0x80;

    if (data == 0)
        set(flag::Z);

    m_rw_device.write(m_reg.HL(), data);
}
