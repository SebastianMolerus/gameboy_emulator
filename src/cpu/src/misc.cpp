#include "cpu_impl.hpp"

uint8_t cpu::cpu_impl::misc()
{
    switch (m_op.m_hex)
    {
    case 0x00:
    case 0xCB:
        NOP();
        break;
    default:
        no_op_defined("misc.cpp");
    }
    return m_op.m_cycles[0];
}

void cpu::cpu_impl::NOP()
{
}