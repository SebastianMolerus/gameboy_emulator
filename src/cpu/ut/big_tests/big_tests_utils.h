#ifndef BIG_TESTS_UTILS_HPP
#define BIG_TESTS_UTILS_HPP

#include <cassert>
#include <cpu.hpp>
#include <cstdint>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <reg.hpp>
#include <span>
#include <vector>

struct alu_data
{
    uint8_t x;
    uint8_t y;
    uint8_t flags;
    struct
    {
        uint8_t value;
        uint8_t flags;
    } result;
};

namespace
{

registers r;
opcode opc;
std::vector<std::tuple<uint16_t, uint8_t, std::string>> c;

bool cb(registers const &reg, opcode const &op, uint8_t)
{
    // PREFIX
    if (op.m_hex == 0xCB)
        return false;
    r = reg;
    opc = op;
    return true;
}

} // namespace

struct bus : public rw_device
{
    std::array<uint8_t, 0x10000> m_ram{0x0};
    cpu m_cpu;

    bus(registers startup_values) : m_cpu{*this, cb, startup_values}
    {
        c.clear();
    }

    bus(std::span<uint8_t> opcodes, registers startup_values) : m_cpu{*this, cb, startup_values}
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    virtual uint8_t read(uint16_t addr)
    {
        c.push_back({addr, m_ram[addr], "read"});
        return m_ram[addr];
    }
    virtual void write(uint16_t addr, uint8_t data)
    {
        c.push_back({addr, data, "write"});
        m_ram[addr] = data;
    }

    void go()
    {
        m_cpu.start();
    }
};

std::vector<alu_data> read_alu_data(std::string alu_file_name);

void validate_opcode(uint8_t hex);
void validate_prefixed(std::string alu_file_name, uint8_t const first_hex);

#endif