#include <cassert>
#include <cpu.hpp>

uint8_t cpu::ld()
{
    switch (m_op.m_hex)
    {
    case 0xF8: // add n to SP and copy it to HL
        return LD_HL_SP_e8();
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
        return LD_REG16_n16();
    case 0xF1: // pop AF
    case 0xC1: // pop BC
    case 0xD1: // pop DE
    case 0xE1: // pop HL
        // pop();
        break;
    case 0xC5: // push BC
    case 0xF5: // push AF
    case 0xD5: // push DE
    case 0xE5: // push HL
        // push();
        break;
    default:
        assert(false);
    }
}

// // 0xF8 : Put SP + n effective address into HL
uint8_t cpu::LD_HL_SP_e8()
{
    reset(flag::Z);
    reset(flag::N);

    uint16_t &SP = m_reg.SP();

    uint8_t val = m_op.m_data[0];
    bool minus = val & 0x80;
    val &= 0x7F;

    if (minus)
        SP -= val;
    else
    {
        uint8_t half_carry_check{static_cast<uint8_t>(SP)};
        half_carry_check &= 0xF;
        half_carry_check += (val & 0xF);
        if (half_carry_check & 0x10)
            set(flag::H);

        uint32_t carry_check{SP};
        carry_check += val;
        if (carry_check & 0x10000)
            set(flag::C);

        SP += val;
    }
    m_reg.HL() = SP;

    return m_op.m_cycles[0] / 4;
}

uint8_t cpu::LD_REG16_n16()
{
    assert(m_op.m_operands[0].m_name);

    uint16_t value = m_op.m_data[1];
    value <<= 8;
    value |= m_op.m_data[0];

    m_reg.get_word(m_op.m_operands[0].m_name) = value;

    return m_op.m_cycles[0] / 4;
}
