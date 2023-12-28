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

uint8_t cpu::cpu_impl::pref_srb()
{
    switch (m_op.m_hex)
    {
    case 0x00:
        RLC_B();
        break;
    default:
        no_op_defined("SRB_pref.cpp");
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
    uint8_t &A = m_reg.A();
    if (A & 0x1)
        set(flag::C);
    std::bitset<8> b{std::rotr(A, 1)};
    A = b.to_ulong();
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

void cpu::cpu_impl::RLC_B()
{
    reset_all_flags();
    uint8_t &B = m_reg.B();
    rotate_l(*this, B);
    if (B == 0)
        set(flag::Z);
}
