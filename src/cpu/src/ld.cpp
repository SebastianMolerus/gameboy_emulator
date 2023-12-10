#include <cassert>
#include <cpu.hpp>

uint8_t cpu::ld()
{
    switch (m_op.m_hex)
    {
    case 0x02:
    case 0x12:
    case 0x22:
    case 0x32:
        return LD_IREG16I_A();
    case 0x0A:
    case 0x1A:
    case 0x2A:
    case 0x3A:
    case 0x4E:
    case 0x5E:
    case 0x6E:
    case 0x7E:
    case 0x46:
    case 0x56:
    case 0x66:
        return LD_REG8_IREG16I();
    case 0x06:
    case 0x16:
    case 0x26:
    case 0x36:
    case 0x0E:
    case 0x1E:
    case 0x2E:
    case 0x3E:
        return LD_REG_n8();
    case 0xF8: // add e8 ( singed data ) to SP and copy it to HL
        return LD_HL_SP_e8();
    case 0xF9:
        return LD_SP_HL();
    case 0x08:
        return LD_Ia16I_SP();
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
        return LD_REG16_n16();
    case 0xE0:
    case 0xF0:
        return LDH();
    case 0xF1: // pop AF
    case 0xC1: // pop BC
    case 0xD1: // pop DE
    case 0xE1: // pop HL
        return pop();
    case 0xC5: // push BC
    case 0xF5: // push AF
    case 0xD5: // push DE
    case 0xE5: // push HL
        return push();
    case 0x7A:
        return LD_REG8_REG8();
    default:
        assert(false);
    }

    static uint8_t ret{0xFF};
    return ret;
}

// // 0xF8 : Put SP + n effective address into HL
uint8_t cpu::LD_HL_SP_e8()
{
    reset(flag::Z);
    reset(flag::N);

    uint16_t SP = m_reg.SP();

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

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_REG16_n16()
{
    assert(m_op.m_operands[0].m_name);

    uint16_t value = m_op.m_data[1];
    value <<= 8;
    value |= m_op.m_data[0];

    m_reg.get_word(m_op.m_operands[0].m_name) = value;

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_Ia16I_SP()
{
    uint16_t addr = m_op.m_data[1];
    addr <<= 8;
    addr |= m_op.m_data[0];

    m_rw_device.write(addr, m_reg.m_SP.m_lo);
    m_rw_device.write(addr + 1, m_reg.m_SP.m_hi);

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_SP_HL()
{
    m_reg.get_word("SP") = m_reg.get_word("HL");
    return m_op.m_cycles[0];
}

uint8_t cpu::push()
{
    assert(m_op.m_operands[0].m_name);

    // Do we have enough space
    assert(m_reg.SP() >= 2);

    // decrease stack first
    m_reg.SP() -= 2;

    uint16_t const source_REG = m_reg.get_word(m_op.m_operands[0].m_name);

    m_rw_device.write(m_reg.SP(), source_REG);
    m_rw_device.write(m_reg.SP() + 1, source_REG >> 8);

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_REG8_REG8()
{
    assert(m_op.m_operands[0].m_name);
    assert(m_op.m_operands[1].m_name);
    m_reg.get_byte(m_op.m_operands[0].m_name) = m_reg.get_byte(m_op.m_operands[1].m_name);
    return m_op.m_cycles[0];
}

uint8_t cpu::pop()
{
    assert(m_op.m_operands[0].m_name);
    uint16_t &target_REG = m_reg.get_word(m_op.m_operands[0].m_name);

    uint8_t const lo = m_rw_device.read(m_reg.SP());
    uint8_t const hi = m_rw_device.read(++m_reg.SP());
    ++m_reg.SP();

    target_REG = hi;
    target_REG <<= 8;
    target_REG |= lo;

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_IREG16I_A()
{
    assert(m_op.m_operands[0].m_name);
    assert(m_op.m_operands[1].m_name);

    const uint16_t addr = m_reg.get_word(m_op.m_operands[0].m_name);
    m_rw_device.write(addr, m_reg.get_byte(m_op.m_operands[1].m_name));

    if (m_op.m_operands[0].m_increment == 1)
        ++m_reg.get_word("HL");

    if (m_op.m_operands[0].m_decrement == 1)
        --m_reg.get_word("HL");

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_REG_n8()
{
    assert(m_op.m_operands[0].m_name);
    uint8_t const value = m_op.m_data[0];

    if (!m_op.m_operands[0].m_immediate)
    {
        // LD [HL], n8
        m_rw_device.write(m_reg.HL(), value);
    }
    else
    {
        m_reg.get_byte(m_op.m_operands[0].m_name) = value;
    }

    return m_op.m_cycles[0];
}

uint8_t cpu::LDH()
{
    // LDH [a8], A -> LDH [a8 + 0xFF00], A
    if (m_op.m_hex == 0xE0)
    {
        m_rw_device.write(m_op.m_data[0] + 0xFF00, m_reg.A());
    }

    // LDH A, [a8] -> LDH A, [a8 + 0xFF00]
    if (m_op.m_hex == 0xF0)
    {
        m_reg.A() = m_rw_device.read(m_op.m_data[0] + 0xFF00);
    }

    return m_op.m_cycles[0];
}

uint8_t cpu::LD_REG8_IREG16I()
{
    assert(m_op.m_operands[0].m_name);
    assert(m_op.m_operands[1].m_name);

    uint8_t const data = m_rw_device.read(m_reg.get_word(m_op.m_operands[1].m_name));
    m_reg.get_byte(m_op.m_operands[0].m_name) = data;

    if (m_op.m_operands[1].m_increment == 1)
        ++m_reg.get_word("HL");

    if (m_op.m_operands[1].m_decrement == 1)
        --m_reg.get_word("HL");

    return m_op.m_cycles[0];
}