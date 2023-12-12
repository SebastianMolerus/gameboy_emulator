#ifndef UTILS_h
#define UTILS_h

#include <cpu.hpp>
#include <cstdint>
#include <span>
#include <vector>

struct rw_mock : public rw_device
{
    std::vector<uint8_t> m_ram;

    // First opcodes are directly mapped to ram
    // to Avoid problems write/read to higher ram values
    rw_mock(std::span<uint8_t> opcodes) : m_ram(0x10000, 0x00)
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
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

using result_type = std::pair<std::vector<registers>, std::vector<uint8_t>>;

static result_type get_cpu_output(int instructions_cc, std::string const &assembly)
{
    result_type result;
    int cc{};

    auto opcodes = transform(assembly);
    rw_mock mock{opcodes};
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