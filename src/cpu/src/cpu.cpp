#include "cpu_impl.hpp"

cpu::cpu_impl::cpu_impl(rw_device &rw_device, cb callback) : m_rw_device{rw_device}, m_callback{callback}
{
}

cpu::cpu_impl::~cpu_impl() = default;

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

bool cpu::cpu_impl::is_carry(uint8_t dest, uint8_t src)
{
    return (dest + src) & 0x100;
}

bool cpu::cpu_impl::is_half_carry(uint8_t dest, uint8_t src)
{
    return ((dest & 0xF) + (src & 0xF)) & 0x10;
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

cpu::cpu(rw_device &rw_device, cb callback) : m_pimpl{std::make_unique<cpu_impl>(rw_device, callback)}
{
}

cpu::~cpu() = default;

void cpu::start()
{
    m_pimpl->start();
}
