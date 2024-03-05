#include "cpu_impl.hpp"
#include "cpu.hpp"
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

extern void check_interrupt(cpu::cpu_impl &cpu);

// ******************************************
//               CPU_IMPL PART
// ******************************************
namespace
{

using instruction = void (cpu::cpu_impl::*)();
using mapper_T = std::unordered_map<const char *, instruction>;
using mapper_const_iter = mapper_T::const_iterator;

const mapper_T mapper{{"LD", &cpu::cpu_impl::ld},         {"LDH", &cpu::cpu_impl::ld},
                      {"PUSH", &cpu::cpu_impl::ld},       {"POP", &cpu::cpu_impl::ld},
                      {"JP", &cpu::cpu_impl::jmp},        {"JR", &cpu::cpu_impl::jmp},
                      {"ADC", &cpu::cpu_impl::alu},       {"ADD", &cpu::cpu_impl::alu},
                      {"NOP", &cpu::cpu_impl::misc},      {"CALL", &cpu::cpu_impl::jmp},
                      {"RET", &cpu::cpu_impl::jmp},       {"RETI", &cpu::cpu_impl::jmp},
                      {"RST", &cpu::cpu_impl::jmp},       {"SUB", &cpu::cpu_impl::alu},
                      {"SBC", &cpu::cpu_impl::alu},       {"AND", &cpu::cpu_impl::alu},
                      {"XOR", &cpu::cpu_impl::alu},       {"OR", &cpu::cpu_impl::alu},
                      {"CP", &cpu::cpu_impl::alu},        {"INC", &cpu::cpu_impl::alu},
                      {"DEC", &cpu::cpu_impl::alu},       {"DAA", &cpu::cpu_impl::alu},
                      {"SCF", &cpu::cpu_impl::alu},       {"CPL", &cpu::cpu_impl::alu},
                      {"CCF", &cpu::cpu_impl::alu},       {"RLCA", &cpu::cpu_impl::srb},
                      {"RLA", &cpu::cpu_impl::srb},       {"RRCA", &cpu::cpu_impl::srb},
                      {"RRA", &cpu::cpu_impl::srb},       {"PREFIX", &cpu::cpu_impl::misc},
                      {"RLC", &cpu::cpu_impl::pref_srb},  {"RRC", &cpu::cpu_impl::pref_srb},
                      {"RL", &cpu::cpu_impl::pref_srb},   {"RR", &cpu::cpu_impl::pref_srb},
                      {"SLA", &cpu::cpu_impl::pref_srb},  {"SRA", &cpu::cpu_impl::pref_srb},
                      {"SWAP", &cpu::cpu_impl::pref_srb}, {"SRL", &cpu::cpu_impl::pref_srb},
                      {"BIT", &cpu::cpu_impl::pref_srb},  {"RES", &cpu::cpu_impl::pref_srb},
                      {"SET", &cpu::cpu_impl::pref_srb},  {"ILLEGAL_D3", &cpu::cpu_impl::misc},
                      {"DI", &cpu::cpu_impl::misc},       {"EI", &cpu::cpu_impl::misc}};

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

cpu::cpu_impl::cpu_impl(rw_device &rw_device, cb callback)
    : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::cpu_impl::adjust_ime()
{
    if (m_IME == IME::ENABLED || m_IME == IME::DISABLED)
        return;

    switch (m_IME)
    {
    case IME::WANT_DISABLE:
        m_IME = IME::DISABLING_IN_PROGRESS;
        break;
    case IME::DISABLING_IN_PROGRESS:
        m_IME = IME::DISABLED;
        break;
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

void cpu::cpu_impl::push_PC()
{
    assert(m_reg.SP() > 0x1);
    m_rw_device.write(--m_reg.SP(), m_reg.PC() >> 8);
    m_rw_device.write(--m_reg.SP(), m_reg.PC());
}

void cpu::cpu_impl::tick()
{
    --m_T_states;
    if (m_T_states > 0)
    {
        // Simulation of instruction processing
        // Need more ticks to complete
        return;
    }

    if (m_callback)
    {
        std::invoke(m_callback, m_reg, m_op);
    }

    auto const saved_pc = m_reg.PC();

    adjust_ime();
    check_interrupt(*this);

    if (m_reg.PC() != saved_pc)
    {
        if (m_callback)
        {
            std::invoke(m_callback, m_reg, m_op);
        }
    }

    m_op = get_opcode(read_byte(), m_pref);

    m_T_states = wait_cycles(*this);

    // fill data needed by opcode
    // only for opcodes which size is greater than 1B
    for (auto i = 0; i < m_op.m_bytes - 1; ++i)
        m_op.m_data[i] = read_byte();

    // Execute opcode
    std::invoke(instruction_lookup(m_op.m_mnemonic), *this);

    // previous opcode was from prefixed table
    // turn this of to get next opcode from non prefixed table
    if (m_pref)
        m_pref = false;

    if (std::strcmp(m_op.m_mnemonic, "PREFIX") == 0)
        m_pref = true;
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
    ss << module_name << ": No operation defined for opcode 0x" << std::hex << (int)m_op.m_hex
       << "\n";
    throw std::runtime_error(ss.str());
}

uint16_t cpu::cpu_impl::combined_data()
{
    uint16_t addr = m_op.m_data[1];
    addr <<= 8;
    addr |= m_op.m_data[0];
    return addr;
}

// ******************************************
//                  CPU PART
// ******************************************
cpu::cpu(rw_device &rw_device, cb callback, registers start_values)
    : m_pimpl{std::make_unique<cpu_impl>(rw_device, callback)}
{
    m_pimpl->m_reg = start_values;
}

cpu::~cpu() = default;

void cpu::tick()
{
    assert(m_pimpl);
    m_pimpl->tick();
}