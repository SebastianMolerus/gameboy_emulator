#include "cpu_impl.hpp"
#include <bit>
#include <bitset>

uint8_t cpu::cpu_impl::srb()
{
    no_op_defined("SRB.cpp");
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

// Circular left rotation
// A[7] is copied to C and A[0]
void cpu::cpu_impl::RLCA()
{
    reset_all_flags();
    uint8_t &A = m_reg.A();
    if (A & 0x80)
        set(flag::C);
    std::bitset<8> b{std::rotl(A, 1)};
    A = b.to_ulong();
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
    if (B & 0x80)
        set(flag::C);
    std::bitset<8> b{std::rotl(B, 1)};
    B = b.to_ulong();
    if (B == 0)
        set(flag::Z);
}
