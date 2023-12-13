#include <cpu.hpp>

uint8_t cpu::misc()
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

uint8_t cpu::NOP()
{
    return 4;
}