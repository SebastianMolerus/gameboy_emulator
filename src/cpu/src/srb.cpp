#include "cpu_impl.hpp"
#include <bit>
#include <bitset>

uint8_t cpu::cpu_impl::srb()
{
    switch (m_op.m_hex)
    {
    case 0x07:
        RLCA();
        break;
    default:
        no_op_defined();
    }
    return 0;
}

void cpu::cpu_impl::RLCA()
{
    reset_all_flags();

    uint8_t &A = m_reg.A();
    if (A & 0x80)
        set(flag::C);
    std::bitset<8> b{std::rotl(A, 1)};
    A = b.to_ulong();
}
