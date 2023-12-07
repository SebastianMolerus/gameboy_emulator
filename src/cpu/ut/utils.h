#ifndef UTILS_h
#define UTILS_h

#include <cpu.hpp>
#include <cstdint>
#include <span>
#include <vector>

struct rw_mock : public rw_device
{
    std::vector<uint8_t> m_ram;
    int const m_read_limit;
    int m_read_cc{};
    rw_mock(int read_limit, std::span<uint8_t> opcodes) : m_read_limit{read_limit}, m_ram(0x10000, 0x00)
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    data read(uint16_t addr) override
    {
        if (m_read_cc == m_read_limit)
            return {0xFF, false};

        return {m_ram[addr], true};

        ++m_read_cc;
    }
    void write(uint16_t addr, uint8_t data) override
    {
        m_ram[addr] = data;
    }
};

#endif