#include "cpu_impl.hpp"

uint8_t cpu::cpu_impl::misc()
{
    switch (m_op.m_hex)
    {
    case 0x00:
        return NOP();
    default:
        no_op_defined();
    }
    return 0;
}

uint8_t cpu::cpu_impl::NOP()
{
    return 4;
}