#ifndef UTILS_h
#define UTILS_h

#include <cpu.hpp>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

using result_type = std::pair<std::vector<registers>, std::vector<uint8_t>>;

struct rw_mock : public rw_device
{
    std::vector<uint8_t> m_ram;

    // First opcodes are directly mapped to ram
    // to Avoid problems write/read to higher ram values
    rw_mock(std::string const &assembly) : m_ram(0x10000, 0x00)
    {
        auto opcodes = transform(assembly);
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    rw_mock(std::span<uint8_t> opcodes) : m_ram(0x10000, 0x00)
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    void add_instruction_at(uint16_t addr, std::string const &instruction)
    {
        auto opcodes = transform(instruction);
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[addr++] = opcodes[i];
    }

    result_type get_cpu_output()
    {
        int nop_cc{2};
        result_type result;
        auto cpu_ptr = std::make_unique<cpu>(
            *this, [&result, &nop_cc](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                if (op.m_hex == 0x0)
                    --nop_cc;

                result.first.push_back(regs);
                result.second.push_back(wait_cycles);

                if (nop_cc == 0)
                    return true;
                return false;
            });
        cpu_ptr->start();
        return result;
    }

    uint8_t read(uint16_t addr) override
    {
        return m_ram[addr];
    }

    void write(uint16_t addr, uint8_t data) override
    {
        m_ram[addr] = data;
    }
};

static result_type get_cpu_output(int instructions_cc, std::string const &assembly)
{
    result_type result;
    int cc{};

    rw_mock mock{assembly};
    cpu cpu{mock, [&result, &cc, &instructions_cc](registers const &regs, opcode const &op, uint8_t wait_cycles) {
                result.first.push_back(regs);
                result.second.push_back(wait_cycles);
                ++cc;
                if (cc == instructions_cc)
                    return true;
                return false;
            }};
    cpu.start();

    return result;
}

#endif