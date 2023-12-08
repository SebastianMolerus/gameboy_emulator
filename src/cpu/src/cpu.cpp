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
        if (rw_device::data d = read_byte(); d.second)
        {
            m_op = get_opcode(d.first);

            auto func = m_mapper.find(m_op.mnemonic);
            assert(func != m_mapper.end());

            for (auto i = 0; i < m_op.bytes - 1; ++i)
                m_op.data[i] = read_byte().first;

            uint8_t const wait_cycles = std::invoke(func->second, *this);

            // ADD WAIT HERE

            // Callback for UTs
            if (m_callback)
                std::invoke(m_callback, m_reg);
        }
        else
        {
            return;
        }
    }
}

rw_device::data cpu::read_byte()
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