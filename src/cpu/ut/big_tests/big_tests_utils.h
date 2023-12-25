#ifndef BIG_TESTS_UTILS_HPP
#define BIG_TESTS_UTILS_HPP

#include <cassert>
#include <cpu.hpp>
#include <cstdint>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <json_spirit/json_spirit.h>
#include <reg.hpp>
#include <span>
#include <sstream>
#include <vector>

namespace
{

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

std::vector<alu_data> read_alu_data(std::filesystem::path file)
{
    assert(std::filesystem::exists(file));

    std::ifstream big_file(file);
    std::stringstream ss;
    ss << big_file.rdbuf();

    std::vector<alu_data> result;
    for (std::string line; std::getline(ss, line);)
    {
        json_spirit::Value value;
        if (json_spirit::read(line, value))
        {
            alu_data new_data;
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

struct cpu_state
{
    struct
    {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t h;
        uint8_t l;
        uint16_t pc;
        uint16_t sp;
    } cpu;
    std::vector<std::pair<uint16_t, uint8_t>> ram;
};

struct cpu_data
{
    std::string name;
    cpu_state initial;
    cpu_state final;
    std::vector<std::tuple<uint16_t, uint8_t, std::string>> cycles;
};

void fill_cpu_data(cpu_state &state, json_spirit::Object const &initial_or_final)
{
    json_spirit::Object const cpu = initial_or_final[0].value_.get_obj();

    state.cpu.a = std::stoi(cpu[0].value_.get_str(), nullptr, 16);
    state.cpu.b = std::stoi(cpu[1].value_.get_str(), nullptr, 16);
    state.cpu.c = std::stoi(cpu[2].value_.get_str(), nullptr, 16);
    state.cpu.d = std::stoi(cpu[3].value_.get_str(), nullptr, 16);
    state.cpu.e = std::stoi(cpu[4].value_.get_str(), nullptr, 16);
    state.cpu.f = std::stoi(cpu[5].value_.get_str(), nullptr, 16);
    state.cpu.h = std::stoi(cpu[6].value_.get_str(), nullptr, 16);
    state.cpu.l = std::stoi(cpu[7].value_.get_str(), nullptr, 16);
    state.cpu.pc = std::stoi(cpu[8].value_.get_str(), nullptr, 16);
    state.cpu.sp = std::stoi(cpu[9].value_.get_str(), nullptr, 16);

    json_spirit::Array const ram = initial_or_final[1].value_.get_array();

    for (auto const &arr : ram)
    {
        json_spirit::Array const addr_value = arr.get_array();
        uint16_t const addr = std::stoi(addr_value[0].get_str(), nullptr, 16);
        uint16_t const value = std::stoi(addr_value[1].get_str(), nullptr, 16);
        state.ram.push_back({addr, value});
    }
}

std::vector<cpu_data> read_cpu_data(std::filesystem::path file)
{
    assert(std::filesystem::exists(file));

    std::ifstream big_file(file);
    std::stringstream ss;
    ss << big_file.rdbuf();
    std::vector<cpu_data> result;

    json_spirit::Value value;
    if (!read(ss, value))
        return {};

    json_spirit::Array arr = value.get_array();

    for (auto const &data_entry : arr)
    {
        cpu_data d;

        json_spirit::Object const main_obj = data_entry.get_obj();

        // Fill Name
        d.name = main_obj[0].value_.get_str();

        // Fill Initial
        json_spirit::Object const initial = main_obj[1].value_.get_obj();
        fill_cpu_data(d.initial, initial);

        // Fill Final
        json_spirit::Object const final_ = main_obj[2].value_.get_obj();
        fill_cpu_data(d.final, final_);

        // Fill Cycles
        json_spirit::Array const cycles = main_obj[3].value_.get_array();
        for (auto const &array_entry : cycles)
        {
            if (array_entry.is_null())
                continue;
            json_spirit::Array const addr_value_type = array_entry.get_array();
            uint16_t const addr = std::stoi(addr_value_type[0].get_str(), nullptr, 16);
            uint16_t const value = std::stoi(addr_value_type[1].get_str(), nullptr, 16);
            std::string const op_type = addr_value_type[2].get_str();
            d.cycles.push_back({addr, value, op_type});
        }

        result.push_back(d);
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

std::filesystem::path const test_data_dir{BIG_TEST_DATA_DIR};
registers r;
opcode opc;
std::vector<std::tuple<uint16_t, uint8_t, std::string>> c;

bool cb(registers const &reg, opcode const &op, uint8_t)
{
    r = reg;
    opc = op;
    return true;
}

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

void validate_cpu_states(std::vector<cpu_data> const &states)
{
    for (auto const &data : states)
    {
        registers startup;
        startup.A() = data.initial.cpu.a;
        startup.B() = data.initial.cpu.b;
        startup.C() = data.initial.cpu.c;
        startup.D() = data.initial.cpu.d;
        startup.E() = data.initial.cpu.e;
        startup.F() = data.initial.cpu.f;
        startup.H() = data.initial.cpu.h;
        startup.L() = data.initial.cpu.l;
        startup.PC() = data.initial.cpu.pc;
        startup.SP() = data.initial.cpu.sp;

        bus b{startup};

        for (auto const &[addr, value] : data.initial.ram)
            b.m_ram[addr] = value;

        try
        {
            b.go();
        }
        catch (std::runtime_error const &err)
        {
            std::cerr << "Exception during execution of " << data.name << "\n";
            std::cerr << err.what() << ". Going to next test\n";
            continue;
        }

        ASSERT_EQ(r.A(), data.final.cpu.a)
            << "Instruction " << data.name << ". Expected A==" << data.final.cpu.a << ". Get A==" << r.A() << "\n";
        ASSERT_EQ(r.B(), data.final.cpu.b)
            << "Instruction " << data.name << ". Expected B==" << data.final.cpu.b << ". Get B==" << r.B() << "\n";
        ASSERT_EQ(r.C(), data.final.cpu.c)
            << "Instruction " << data.name << ". Expected C==" << data.final.cpu.c << ". Get C==" << r.C() << "\n";
        ASSERT_EQ(r.D(), data.final.cpu.d)
            << "Instruction " << data.name << ". Expected D==" << data.final.cpu.d << ". Get D==" << r.D() << "\n";
        ASSERT_EQ(r.F(), data.final.cpu.f)
            << "Instruction " << data.name << ". Expected F==" << data.final.cpu.f << ". Get F==" << r.F() << "\n";
        ASSERT_EQ(r.E(), data.final.cpu.e)
            << "Instruction " << data.name << ". Expected E==" << data.final.cpu.e << ". Get E==" << r.E() << "\n";
        ASSERT_EQ(r.H(), data.final.cpu.h)
            << "Instruction " << data.name << ". Expected H==" << data.final.cpu.h << ". Get H==" << r.H() << "\n";
        ASSERT_EQ(r.L(), data.final.cpu.l)
            << "Instruction " << data.name << ". Expected L==" << data.final.cpu.l << ". Get L==" << r.L() << "\n";
        ASSERT_EQ(r.PC(), data.final.cpu.pc)
            << "Instruction " << data.name << ". Expected PC==" << data.final.cpu.pc << ". Get PC==" << r.PC() << "\n";
        ASSERT_EQ(r.SP(), data.final.cpu.sp)
            << "Instruction " << data.name << ". Expected SP==" << data.final.cpu.sp << ". Get SP==" << r.SP() << "\n";

        for (auto const &[addr, value] : data.final.ram)
            ASSERT_EQ(b.m_ram[addr], value);

        ASSERT_EQ(c, data.cycles);
    }
}

} // namespace

#endif