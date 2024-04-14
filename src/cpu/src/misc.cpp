#include "cpu_impl.hpp"
#include <iostream>

void cpu::cpu_impl::misc()
{
    switch (m_op.m_hex)
    {
    case 0x00:
        NOP();
        break;
    case 0x76:
        HALT();
        break;
    case 0x10:
        STOP();
        break;
    case 0xF3:
        m_IME = IME::DISABLED;
        break;
    case 0xFB:
        m_IME = IME::WANT_ENABLE;
        break;
    default:
        no_op_defined("misc.cpp");
    }
}

void cpu::cpu_impl::NOP()
{
}

void cpu::cpu_impl::STOP()
{
    m_is_stopped = true;
}

void cpu::cpu_impl::HALT()
{
    m_is_halted = true;
}