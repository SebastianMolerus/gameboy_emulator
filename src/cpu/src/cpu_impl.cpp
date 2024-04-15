#include "cpu_impl.hpp"
#include "cpu.hpp"
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <fstream>

extern void check_interrupt(cpu::cpu_impl &cpu);

// ******************************************
//               CPU_IMPL PART
// ******************************************
namespace
{

using instruction = void (cpu::cpu_impl::*)();
using mapper_T = std::unordered_map<const char *, instruction>;
using mapper_const_iter = mapper_T::const_iterator;

const mapper_T mapper{
    {"LD", &cpu::cpu_impl::ld},         {"LDH", &cpu::cpu_impl::ld},          {"PUSH", &cpu::cpu_impl::ld},      {"POP", &cpu::cpu_impl::ld},
    {"JP", &cpu::cpu_impl::jmp},        {"JR", &cpu::cpu_impl::jmp},          {"ADC", &cpu::cpu_impl::alu},      {"ADD", &cpu::cpu_impl::alu},
    {"NOP", &cpu::cpu_impl::misc},      {"CALL", &cpu::cpu_impl::jmp},        {"RET", &cpu::cpu_impl::jmp},      {"RETI", &cpu::cpu_impl::jmp},
    {"RST", &cpu::cpu_impl::jmp},       {"SUB", &cpu::cpu_impl::alu},         {"SBC", &cpu::cpu_impl::alu},      {"AND", &cpu::cpu_impl::alu},
    {"XOR", &cpu::cpu_impl::alu},       {"OR", &cpu::cpu_impl::alu},          {"CP", &cpu::cpu_impl::alu},       {"INC", &cpu::cpu_impl::alu},
    {"DEC", &cpu::cpu_impl::alu},       {"DAA", &cpu::cpu_impl::alu},         {"SCF", &cpu::cpu_impl::alu},      {"CPL", &cpu::cpu_impl::alu},
    {"CCF", &cpu::cpu_impl::alu},       {"RLCA", &cpu::cpu_impl::srb},        {"RLA", &cpu::cpu_impl::srb},      {"RRCA", &cpu::cpu_impl::srb},
    {"RRA", &cpu::cpu_impl::srb},       {"PREFIX", &cpu::cpu_impl::misc},     {"RLC", &cpu::cpu_impl::pref_srb}, {"RRC", &cpu::cpu_impl::pref_srb},
    {"RL", &cpu::cpu_impl::pref_srb},   {"RR", &cpu::cpu_impl::pref_srb},     {"SLA", &cpu::cpu_impl::pref_srb}, {"SRA", &cpu::cpu_impl::pref_srb},
    {"SWAP", &cpu::cpu_impl::pref_srb}, {"SRL", &cpu::cpu_impl::pref_srb},    {"BIT", &cpu::cpu_impl::pref_srb}, {"RES", &cpu::cpu_impl::pref_srb},
    {"SET", &cpu::cpu_impl::pref_srb},  {"ILLEGAL_D3", &cpu::cpu_impl::misc}, {"DI", &cpu::cpu_impl::misc},      {"STOP", &cpu::cpu_impl::misc},
    {"EI", &cpu::cpu_impl::misc},       {"HALT", &cpu::cpu_impl::misc}};

instruction instruction_lookup(const char *mnemonic)
{
    auto iter = mapper.find(mnemonic);
    if (iter == mapper.end())
    {
        std::stringstream ss;
        ss << "CPU: cannot find [" << mnemonic << "] in mapped functions.\n";
        throw std::runtime_error(ss.str());
    }
    return iter->second;
}

uint8_t wait_cycles(cpu::cpu_impl &c)
{
    switch (c.m_op.m_hex)
    {
    case 0x20:
    case 0x30:
    case 0x28:
    case 0x38:
    case 0xC0:
    case 0xD0:
    case 0xC2:
    case 0xD2:
    case 0xC4:
    case 0xD4:
    case 0xC8:
    case 0xD8:
    case 0xCA:
    case 0xDA:
    case 0xCC:
    case 0xDC:
        if (!c.m_reg.check_condition(c.m_op.m_operands[0].m_name))
            return c.m_op.m_cycles[1];
    default:
        return c.m_op.m_cycles[0];
    }
}

} // namespace

cpu::cpu_impl::cpu_impl(rw_device &rw_device, cb callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::cpu_impl::adjust_ime()
{
    if (m_IME == IME::ENABLED || m_IME == IME::DISABLED)
        return;

    switch (m_IME)
    {
    case IME::WANT_ENABLE:
        m_IME = IME::ENABLING_IN_PROGRESS;
        break;
    case IME::ENABLING_IN_PROGRESS:
        m_IME = IME::ENABLED;
        break;
    default:
        break;
    }
}

bool cpu::cpu_impl::is_int_pending()
{
    uint8_t const IF = m_rw_device.read(0xFF0F, device::CPU, true);
    uint8_t const IE = m_rw_device.read(0xFFFF, device::CPU, true);
    return IF & IE;
}

void cpu::cpu_impl::SERIAL_INT()
{
    uint8_t IF = m_rw_device.read(0xFF0F);
    setbit(IF, 3);
    m_rw_device.write(0xFF0F, IF);
}

void cpu::cpu_impl::TIMER_INT()
{
    uint8_t IF = m_rw_device.read(0xFF0F);
    setbit(IF, 2);
    m_rw_device.write(0xFF0F, IF);
}

void cpu::cpu_impl::push_PC()
{
    m_rw_device.write(--m_reg.SP(), m_reg.PC() >> 8);
    m_rw_device.write(--m_reg.SP(), m_reg.PC());
}

void cpu::cpu_impl::serial_transfer()
{
    constexpr int M128{128 * 4};
    static int serial_transfer_cc{M128};

    uint8_t const SC = m_rw_device.read(0xFF02);
    if (checkbit(SC, 7))
    {
        if (checkbit(SC, 0))
        {
            --serial_transfer_cc;
            if (!serial_transfer_cc)
            {
                m_rw_device.write(0xFF02, (SC & 0x7F));
                SERIAL_INT();
                serial_transfer_cc = M128;
            }
        }
    }
    else
        serial_transfer_cc = M128;
}

void cpu::cpu_impl::tick()
{
    if (m_is_stopped)
        return;

    timer();

    --m_T_states;
    if (m_T_states > 0)
    {
        // Simulation of instruction processing
        // Need more ticks to complete
        return;
    }

    check_interrupt(*this);

    // Interrupt takes some cycles
    if (m_interrupt_wait != 0)
    {
        --m_interrupt_wait;
        return;
    }

    serial_transfer();

    if (m_is_halted && is_int_pending() && m_IME == IME::DISABLED)
        m_is_halted = false;

    if (m_is_halted)
    {
        return;
    }

    adjust_ime();

    uint16_t const curr_PC = m_reg.PC();

    m_op = get_opcode(read_byte(), false);
    m_T_states = wait_cycles(*this);
    if (std::strcmp(m_op.m_mnemonic, "PREFIX") == 0)
    {
        m_T_states = 4; // 4 clocks for 0xCB ( Prefix )
        m_op = get_opcode(read_byte(), true);

        if (m_op.m_immediate)
            m_T_states += 8;
        else
            m_T_states += 16;
    }

    // fill data needed by opcode
    // only for opcodes which size is greater than 1B
    for (auto i = 0; i < m_op.m_bytes - 1; ++i)
        m_op.m_data[i] = read_byte();

    // Execute opcode
    std::invoke(instruction_lookup(m_op.m_mnemonic), *this);

    std::invoke(m_callback, m_reg, m_op);
}

uint8_t cpu::cpu_impl::read_byte()
{
    return m_rw_device.read(m_reg.PC()++);
}

void cpu::cpu_impl::set(flag f)
{
    m_reg.m_AF.m_lo |= f;
}

void cpu::cpu_impl::reset(flag f)
{
    m_reg.m_AF.m_lo &= ~f;
}

void cpu::cpu_impl::reset_all_flags()
{
    m_reg.F() = 0;
}

bool cpu::cpu_impl::is_carry_on_addition_byte(uint8_t dest, uint8_t src)
{
    return (dest + src) & 0x100;
}

bool cpu::cpu_impl::is_half_carry_on_addition_byte(uint8_t dest, uint8_t src)
{
    return ((src & 0xF) + (dest & 0xF)) & 0x10;
}

bool cpu::cpu_impl::is_carry_on_addition_word(uint16_t dst, uint16_t src)
{
    return (dst + src) & 0x10000;
}

bool cpu::cpu_impl::is_half_carry_on_addition_word(uint16_t dst, uint16_t src)
{
    return ((dst & 0xFFF) + (src & 0xFFF)) & 0x1000;
}

bool cpu::cpu_impl::is_carry_on_substraction_byte(uint8_t dest, uint8_t src)
{
    return src > dest;
}

bool cpu::cpu_impl::is_half_carry_on_substraction_byte(uint8_t dest, uint8_t src)
{
    return (src & 0xF) > (dest & 0xF);
}

void cpu::cpu_impl::no_op_defined(std::string module_name)
{
    std::stringstream ss;
    ss << module_name << ": No operation defined for opcode 0x" << std::hex << (int)m_op.m_hex << "\n";
    throw std::runtime_error(ss.str());
}

uint16_t cpu::cpu_impl::combined_data()
{
    uint16_t addr = m_op.m_data[1];
    addr <<= 8;
    addr |= m_op.m_data[0];
    return addr;
}

void cpu::cpu_impl::resume()
{
    m_is_stopped = false;
}

// ******************************************
//                  CPU PART
// ******************************************
cpu::cpu(rw_device &rw_device, cb callback, registers start_values) : m_pimpl{std::make_unique<cpu_impl>(rw_device, callback)}
{
    m_pimpl->m_reg = start_values;
}

cpu::~cpu() = default;

void cpu::tick()
{
    assert(m_pimpl);
    m_pimpl->tick();
}

void cpu::resume()
{
    m_pimpl->resume();
}
