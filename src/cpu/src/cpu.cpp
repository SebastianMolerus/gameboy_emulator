#include <cassert>
#include <cpu.hpp>

const std::unordered_map<const char *, cpu::processing_func> cpu::m_mapper{
    {"LD", &cpu::ld}, {"LDH", &cpu::ld}, {"PUSH", &cpu::push}};

cpu::cpu(rw_device &rw_device, callback_sig callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::start()
{
    load_opcodes();
    while (1)
    {
        m_op = get_opcode(read_byte());

        auto func = m_mapper.find(m_op.mnemonic);
        assert(func != m_mapper.end());

        for (auto i = 0; i < m_op.bytes - 1; ++i)
            m_op.data[i] = read_byte();

        uint8_t const wait_cycles = std::invoke(func->second, this);

        // ADD WAIT HERE

        // Callback for UTs
        if (m_callback)
        {
            bool should_quit = std::invoke(m_callback, m_regs);
            if (should_quit)
                break;
        }
    }
}

uint8_t cpu::read_byte()
{
    return m_rw_device.read(PC()++);
}

void cpu::set(flag f)
{
    m_regs.m_AF.m_lo |= f;
}

void cpu::reset(flag f)
{
    m_regs.m_AF.m_lo &= ~f;
}

uint16_t &cpu::PC()
{
    return m_regs.m_PC.m_u16;
}

uint16_t &cpu::SP()
{
    return m_regs.m_SP.m_u16;
}

uint16_t &cpu::HL()
{
    return m_regs.m_HL.m_u16;
}

uint16_t &cpu::BC()
{
    return m_regs.m_BC.m_u16;
}

uint8_t &cpu::A()
{
    return m_regs.m_AF.m_hi;
}

uint8_t cpu::ld()
{
    switch (m_op.hex)
    {
    case 0xF8:
        LD_HL_SP_e8();
        break;
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

// add n8 to SP and copy it to HL
void cpu::LD_HL_SP_e8()
{
    reset(flag::Z);
    reset(flag::N);

    uint8_t val = m_op.data[0];
    bool minus = val & 0x80;
    val &= 0x7F;

    uint16_t &sp = SP();

    if (minus)
        sp -= val;
    else
    {
        uint8_t half_carry_check{static_cast<uint8_t>(sp)};
        half_carry_check &= 0xF;
        half_carry_check += (val & 0xF);
        if (half_carry_check & 0x10)
            set(flag::H);

        uint32_t carry_check{sp};
        carry_check += val;
        if (carry_check & 0x10000)
            set(flag::C);

        sp += val;
    }
    HL() = sp;
}

uint8_t cpu::push()
{
    return 0;
}
