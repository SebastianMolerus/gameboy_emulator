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
    case 0xC3:
        return JP_nn();
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
    uint16_t addr{m_op.m_data[1]};
    addr <<= 8;
    addr |= m_op.m_data[0];

    m_reg.PC() = addr;
    return m_op.m_cycles[0];
}