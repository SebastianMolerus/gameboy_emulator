#ifndef BIG_TESTS_UTILS_HPP
#define BIG_TESTS_UTILS_HPP

#include <cassert>
#include <cpu.hpp>
#include <cstdint>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <json_spirit/json_spirit.h>
#include <reg.hpp>
#include <span>
#include <sstream>
#include <vector>

namespace
{

struct data
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

std::vector<data> read_big_data(std::filesystem::path file)
{
    assert(std::filesystem::exists(file));

    std::ifstream big_file(file);
    std::stringstream ss;
    ss << big_file.rdbuf();

    std::vector<data> result;
    for (std::string line; std::getline(ss, line);)
    {
        json_spirit::Value value;
        if (json_spirit::read(line, value))
        {
            data new_data;
            json_spirit::Object const main_obj = value.get_obj();
            new_data.x = std::stoi(main_obj[0].value_.get_str(), nullptr, 16);
            new_data.y = std::stoi(main_obj[1].value_.get_str(), nullptr, 16);
            new_data.flags = std::stoi(main_obj[2].value_.get_str(), nullptr, 16);

            json_spirit::Object const second_obj = main_obj[3].value_.get_obj();
            new_data.result.value = std::stoi(second_obj[0].value_.get_str(), nullptr, 16);
            new_data.result.flags = std::stoi(second_obj[1].value_.get_str(), nullptr, 16);
            result.push_back(new_data);
        }
    }

    return result;
}

#define DIAGNOSTIC                                                                                                     \
    std::stringstream diagnostic_message;                                                                              \
    diagnostic_message << "Diagnostic:"                                                                                \
                       << "\n"                                                                                         \
                       << "Start values: " << (int)data.x << " " << (int)data.y << " " << (int)data.flags << "\n";     \
    diagnostic_message << "Expected values: " << (int)data.result.value << " " << (int)data.result.flags << "\n";      \
    diagnostic_message << "Current values: " << (int)r.A() << " " << (int)r.F() << "\n";

std::filesystem::path const alu_test_data{ALU_TEST_PATH};
registers r;
opcode opc;

bool cb(registers const &reg, opcode const &op, uint8_t)
{
    if (op.m_hex == 0)
        return true;
    r = reg;
    opc = op;
    return false;
}

struct bus : public rw_device
{
    std::array<uint8_t, 10> m_ram{};
    cpu m_cpu;

    bus(std::span<uint8_t> opcodes, registers startup_values) : m_cpu{*this, cb, startup_values}
    {
        for (auto i = 0; i < opcodes.size(); ++i)
            m_ram[i] = opcodes[i];
    }

    virtual uint8_t read(uint16_t addr)
    {
        return m_ram[addr];
    }
    virtual void write(uint16_t addr, uint8_t data)
    {
        m_ram[addr] = data;
    }

    void go()
    {
        m_cpu.start();
    }
};

} // namespace

#endif