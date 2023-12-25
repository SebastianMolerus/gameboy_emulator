#include "cpu_impl.hpp"
#include "cpu.hpp"
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// ******************************************
//               CPU_IMPL PART
// ******************************************
const std::unordered_map<const char *, cpu::cpu_impl::processing_func> cpu::cpu_impl::m_mapper{
    {"LD", &cpu::cpu_impl::ld},    {"LDH", &cpu::cpu_impl::ld},  {"PUSH", &cpu::cpu_impl::ld},
    {"POP", &cpu::cpu_impl::ld},   {"JP", &cpu::cpu_impl::jmp},  {"JR", &cpu::cpu_impl::jmp},
    {"ADC", &cpu::cpu_impl::alu},  {"ADD", &cpu::cpu_impl::alu}, {"NOP", &cpu::cpu_impl::misc},
    {"CALL", &cpu::cpu_impl::jmp}, {"RET", &cpu::cpu_impl::jmp}, {"RETI", &cpu::cpu_impl::jmp},
    {"RST", &cpu::cpu_impl::jmp},  {"SUB", &cpu::cpu_impl::alu}, {"SBC", &cpu::cpu_impl::alu},
    {"AND", &cpu::cpu_impl::alu},  {"XOR", &cpu::cpu_impl::alu}};

cpu::cpu_impl::cpu_impl(rw_device &rw_device, cb callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::cpu_impl::start()
{
    while (1)
    {
        uint8_t const hex = read_byte();
        m_op = get_opcode(hex);

        auto func = m_mapper.find(m_op.m_mnemonic);
        if (func == m_mapper.end())
        {
            std::stringstream ss;
            ss << "CPU: cannot find [" << m_op.m_mnemonic << "] in mapped functions.";
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
    reset(flag::N);
    reset(flag::C);
    reset(flag::H);
    reset(flag::Z);
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

void cpu::cpu_impl::no_op_defined()
{
    std::stringstream ss;
    ss << __func__ << ": no operation defined for opcode 0x" << std::hex << (int)m_op.m_hex;
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