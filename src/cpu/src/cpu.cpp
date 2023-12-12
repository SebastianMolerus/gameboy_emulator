#include <cassert>
#include <cpu.hpp>
#include <sstream>
#include <stdexcept>

const std::unordered_map<const char *, cpu::processing_func> cpu::m_mapper{{"LD", &cpu::ld},   {"LDH", &cpu::ld},
                                                                           {"PUSH", &cpu::ld}, {"POP", &cpu::ld},
                                                                           {"JR", &cpu::jmp},  {"ADD", &cpu::arith}};

cpu::cpu(rw_device &rw_device, cb callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

void cpu::start()
{
    load_opcodes();
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

uint8_t cpu::read_byte()
{
    return m_rw_device.read(m_reg.PC()++);
}

void cpu::set(flag f)
{
    m_reg.m_AF.m_lo |= f;
}

void cpu::reset(flag f)
{
    m_reg.m_AF.m_lo &= ~f;
}

bool cpu::is_carry(uint8_t dest, uint8_t src)
{
    return (dest + src) & 0x100;
}

bool cpu::is_half_carry(uint8_t dest, uint8_t src)
{
    return ((dest & 0xF) + (src & 0xF)) & 0x10;
}