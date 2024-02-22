#include "cpu_impl.hpp"
#include <string>

namespace
{

void pop_PC(cpu::cpu_impl &c)
{
    assert(c.m_reg.SP() < 0xFFFE);
    c.m_reg.m_PC.m_lo = c.m_rw_device.read(c.m_reg.SP()++);
    c.m_reg.m_PC.m_hi = c.m_rw_device.read(c.m_reg.SP()++);
}

} // namespace

void cpu::cpu_impl::jmp()
{
    switch (m_op.m_hex)
    {
    case 0x18:
        return JR_e8();
    case 0x20:
    case 0x30:
    case 0x28:
    case 0x38:
        return JR_CC_e8();
    case 0xC2:
    case 0xD2:
    case 0xCA:
    case 0xDA:
        return JP_CC_a16();
    case 0xC4:
    case 0xD4:
    case 0xCC:
    case 0xDC:
        return CALL_CC_a16();
    case 0xCD:
        return CALL_a16();
    case 0xC3:
        return JP_nn();
    case 0xC9:
        return RET();
    case 0xC0:
    case 0xC8:
    case 0xD0:
    case 0xD8:
        return RET_CC();
    case 0xC7:
    case 0xCF:
    case 0xD7:
    case 0xDF:
    case 0xE7:
    case 0xEF:
    case 0xF7:
    case 0xFF:
        return RST_nn();
    case 0xD9:
        return RETI();
    case 0xE9:
        return JP_HL();
    default:
        no_op_defined("jmp.cpp");
    }
}

void cpu::cpu_impl::JR_CC_e8()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
        JR_e8(); // jump
}

void cpu::cpu_impl::JR_e8()
{
    uint8_t e8 = m_op.m_data[0];
    if (e8 & 0x80)
    {
        e8 -= 1;
        m_reg.PC() -= (uint8_t)(~e8);
    }
    else
        m_reg.PC() += e8;
}

void cpu::cpu_impl::JP_nn()
{
    m_reg.PC() = combined_data();
}

void cpu::cpu_impl::JP_HL()
{
    m_reg.PC() = m_reg.HL();
}

void cpu::cpu_impl::JP_CC_a16()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
        JP_nn(); // jump
}

void cpu::cpu_impl::CALL_CC_a16()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
        CALL_a16(); // call
}

void cpu::cpu_impl::CALL_a16()
{
    push_PC();
    JP_nn();
}

void cpu::cpu_impl::RET()
{
    pop_PC(*this);
}

void cpu::cpu_impl::RET_CC()
{
    assert(m_op.m_operands[0].m_name);
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
        RET();
}

void cpu::cpu_impl::RETI()
{
    m_IME = IME::ENABLED;
    RET();
}

void cpu::cpu_impl::RST_nn()
{
    assert(m_op.m_operands[0].m_name);
    push_PC();
    m_reg.PC() = std::stoi({m_op.m_operands[0].m_name + 1}, nullptr, 16);
}