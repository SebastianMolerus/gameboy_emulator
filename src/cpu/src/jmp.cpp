#include <cpu.hpp>
#include <sstream>

uint8_t cpu::jmp()
{
    switch (m_op.m_hex)
    {
    case 0x20:
        return JR_NZ_e8();
    case 0x30:
        return JR_NC_e8();
    case 0xC3:
        return JP_nn();
    default:
        no_op_defined();
    }
    return 0;
}

uint8_t cpu::JR_NZ_e8()
{
    if (!m_reg.is_flag_set(flag::Z))
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

uint8_t cpu::JP_nn()
{
    uint16_t addr{m_op.m_data[1]};
    addr <<= 8;
    addr |= m_op.m_data[0];

    m_reg.PC() = addr;
    return m_op.m_cycles[0];
}

uint8_t cpu::JR_NC_e8()
{
    if (!m_reg.is_flag_set(flag::C))
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