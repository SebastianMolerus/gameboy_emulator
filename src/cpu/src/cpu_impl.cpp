#include "cpu_impl.hpp"
#include "cpu.hpp"
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// ******************************************
//               CPU_IMPL PART
// ******************************************
const std::unordered_map<const char *, cpu::cpu_impl::processing_func> cpu::cpu_impl::m_mapper{
    {"LD", &cpu::cpu_impl::ld},         {"LDH", &cpu::cpu_impl::ld},       {"PUSH", &cpu::cpu_impl::ld},
    {"POP", &cpu::cpu_impl::ld},        {"JP", &cpu::cpu_impl::jmp},       {"JR", &cpu::cpu_impl::jmp},
    {"ADC", &cpu::cpu_impl::alu},       {"ADD", &cpu::cpu_impl::alu},      {"NOP", &cpu::cpu_impl::misc},
    {"CALL", &cpu::cpu_impl::jmp},      {"RET", &cpu::cpu_impl::jmp},      {"RETI", &cpu::cpu_impl::jmp},
    {"RST", &cpu::cpu_impl::jmp},       {"SUB", &cpu::cpu_impl::alu},      {"SBC", &cpu::cpu_impl::alu},
    {"AND", &cpu::cpu_impl::alu},       {"XOR", &cpu::cpu_impl::alu},      {"OR", &cpu::cpu_impl::alu},
    {"CP", &cpu::cpu_impl::alu},        {"INC", &cpu::cpu_impl::alu},      {"DEC", &cpu::cpu_impl::alu},
    {"DAA", &cpu::cpu_impl::alu},       {"SCF", &cpu::cpu_impl::alu},      {"CPL", &cpu::cpu_impl::alu},
    {"CCF", &cpu::cpu_impl::alu},       {"RLCA", &cpu::cpu_impl::srb},     {"RLA", &cpu::cpu_impl::srb},
    {"RRCA", &cpu::cpu_impl::srb},      {"RRA", &cpu::cpu_impl::srb},      {"PREFIX", &cpu::cpu_impl::misc},
    {"RLC", &cpu::cpu_impl::pref_srb},  {"RRC", &cpu::cpu_impl::pref_srb}, {"RL", &cpu::cpu_impl::pref_srb},
    {"RR", &cpu::cpu_impl::pref_srb},   {"SLA", &cpu::cpu_impl::pref_srb}, {"SRA", &cpu::cpu_impl::pref_srb},
    {"SWAP", &cpu::cpu_impl::pref_srb}, {"SRL", &cpu::cpu_impl::pref_srb}, {"BIT", &cpu::cpu_impl::pref_srb},
    {"RES", &cpu::cpu_impl::pref_srb},  {"SET", &cpu::cpu_impl::pref_srb}};

using mapping_iter = std::unordered_map<const char *, cpu::cpu_impl::processing_func>::const_iterator;

cpu::cpu_impl::cpu_impl(rw_device &rw_device, cb callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::cpu_impl::start()
{
    constexpr uint8_t PREFIX_OPCODE{0xCB};

    while (1)
    {
        mapping_iter func;
        uint8_t const hex{read_byte()};

        m_op = get_opcode(hex, m_pref);

        if (m_pref)
            m_pref = false;

        func = m_mapper.find(m_op.m_mnemonic);
        if (func == m_mapper.end())
        {
            std::stringstream ss;
            ss << "CPU: cannot find [" << m_op.m_mnemonic << "] in mapped functions.\n";
            if (m_pref)
                ss << "[PREFIXED]\n";
            throw std::runtime_error(ss.str());
        }

        for (auto i = 0; i < m_op.m_bytes - 1; ++i)
            m_op.m_data[i] = read_byte();

        uint8_t const wait_cycles = std::invoke(func->second, *this);

        // ADD WAIT HERE

        if (m_callback)
        {
            if (bool res = std::invoke(m_callback, m_reg, m_op, wait_cycles); res)
                return;
        }

        if (std::strcmp(m_op.m_mnemonic, "PREFIX") == 0)
            m_pref = true;
    }
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

void cpu::cpu_impl::push_PC()
{
    assert(m_reg.SP() > 0x1);
    m_rw_device.write(--m_reg.SP(), m_reg.PC() >> 8);
    m_rw_device.write(--m_reg.SP(), m_reg.PC());
}

void cpu::cpu_impl::pop_PC()
{
    assert(m_reg.SP() < 0xFFFE);
    m_reg.m_PC.m_lo = m_rw_device.read(m_reg.SP()++);
    m_reg.m_PC.m_hi = m_rw_device.read(m_reg.SP()++);
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

void cpu::start()
{
    assert(m_pimpl);
    m_pimpl->start();
}