#include <cpu.hpp>

uint8_t cpu::jmp()
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
    case 0xE9:
        return JP_HL();
    default:
        no_op_defined();
    }
    return 0;
}

uint8_t cpu::JR_CC_e8()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
    {
        // jump
        uint8_t e8 = m_op.m_data[0];
        if (e8 & 0x80)
            m_reg.PC() -= (e8 & 0x7F);
        else
            m_reg.PC() += e8;
        return m_op.m_cycles[0];
    }
    else
    {
        // no jump
        return m_op.m_cycles[1];
    }
}

uint8_t cpu::JR_e8()
{
    uint8_t e8 = m_op.m_data[0];
    if (e8 & 0x80)
        m_reg.PC() -= (e8 & 0x7F);
    else
        m_reg.PC() += e8;
    return m_op.m_cycles[0];
}

uint8_t cpu::JP_nn()
{
    m_reg.PC() = combined_data();
    return m_op.m_cycles[0];
}

uint8_t cpu::JP_CC_a16()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
    {
        // jump
        m_reg.PC() = combined_data();
        return m_op.m_cycles[0];
    }
    else
    {
        // no jump
        return m_op.m_cycles[1];
    }
}

uint8_t cpu::JP_HL()
{
    m_reg.PC() = m_reg.HL();
    return m_op.m_cycles[0];
}

uint8_t cpu::CALL_CC_a16()
{
    if (m_reg.check_condition(m_op.m_operands[0].m_name))
    {
        // call
        push_PC();
        m_reg.PC() = combined_data();
        return m_op.m_cycles[0];
    }
    else
    {
        // no call
        return m_op.m_cycles[1];
    }
}

uint8_t cpu::CALL_a16()
{
    push_PC();
    m_reg.PC() = combined_data();
    return m_op.m_cycles[0];
}