#include <cassert>
#include <cpu.hpp>

const std::unordered_map<const char *, cpu::processing_func> cpu::m_mapper{{"LD", &cpu::ld}, {"LDH", &cpu::ld}};

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
        assert(func != m_mapper.end());

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