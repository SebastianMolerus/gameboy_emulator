#include <cpu.hpp>
#include <sstream>

uint8_t cpu::jmp()
{
    switch (m_op.m_hex)
    {
    case 0x20:
        return JR_NZ_e8();
    default:
        std::stringstream ss;
        ss << __func__ << ": no operation defined for opcode 0x" << std::hex << (int)m_op.m_hex;
        throw std::runtime_error(ss.str());
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