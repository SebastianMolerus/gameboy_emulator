#include "cpu_impl.hpp"

void cpu::cpu_impl::misc()
{
    switch (m_op.m_hex)
    {
    case 0x00:
    case 0xCB:
        NOP();
        break;
    case 0xD3:
        ILLEGAL();
        break;
    case 0xF3:
        m_IME = IME::WANT_DISABLE;
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

void cpu::cpu_impl::ILLEGAL()
{
    throw std::runtime_error("ILLEGAL INSTRUCTION WERE EXECUTED\n");
}