#include <cassert>
#include <cpu.hpp>

uint8_t cpu::ld()
{
    switch (m_op.hex)
    {
    case 0xF8: // add n to SP and copy it to HL
        return LD_HL_SP_e8();
    case 0xF9:
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
    case 0x02:
    case 0x12:
    case 0x22:
    case 0x32:
    case 0x06:
    case 0x16:
    case 0x26:
    case 0x36:
    case 0x08:
    case 0x0A:
    case 0x1A:
    case 0x2A:
    case 0x3A:
    case 0x0E:
    case 0x1E:
    case 0x2E:
    case 0x3E:
    case 0xE0:
    case 0xF0:
    case 0xE2:
    case 0xF2:
    case 0xEA:
    case 0xFA:
    case 0x40 ... 0x7F:
        assert(m_op.hex != 0x76); // Halt;
        // load_source();
        break;
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

    uint8_t val = m_op.data[0];
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

    return m_op.cycles[0] / 4;
}
