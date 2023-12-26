#include <array>
#include <cassert>
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <json_spirit/json_spirit.h>
#include <reg.hpp>
#include <span>

std::filesystem::path alu_test_data{TEST_DATA_PATH};

struct bus : public rw_device
{
    std::array<uint8_t, 10> m_ram{};
    cpu m_cpu;

    bus(std::span<uint8_t> opcodes) : m_cpu{*this, cb}
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

    void set_flags(uint8_t flags)
    {
        m_cpu.set_flags(flags);
    }
};

int main()
{
    using namespace std;
    using json_spirit::Value;

    assert(filesystem::exists(alu_test_data / "alu_tests" / "v1"));

    std::ifstream is(alu_test_data / "alu_tests" / "v1" / "add.json");

    for (std::string line; std::getline(is, line);)
    {
        std::cout << line << '\n';
        Value value;
        if (read(line, value))
        {
            json_spirit::Object const main_obj = value.get_obj();

            json_spirit::Value_type type0 = main_obj[0].value_.type(); // str
            json_spirit::Value_type type1 = main_obj[1].value_.type(); // str
            json_spirit::Value_type type2 = main_obj[2].value_.type(); // str
            json_spirit::Value_type type3 = main_obj[3].value_.type(); // obj

            uint8_t const x = std::stoi(main_obj[0].value_.get_str(), nullptr, 16);
            uint8_t const y = std::stoi(main_obj[1].value_.get_str(), nullptr, 16);
            uint8_t const flags_pre = std::stoi(main_obj[2].value_.get_str(), nullptr, 16);

            json_spirit::Object const second_obj = main_obj[3].value_.get_obj();

            uint8_t const value = std::stoi(second_obj[0].value_.get_str(), nullptr, 16);
            uint8_t const flags_post = std::stoi(second_obj[1].value_.get_str(), nullptr, 16);

            std::array<uint8_t, 5> opcodes{0x3E, x, 0x06, y, 0x80};
            bus b{opcodes};

            b.set_flags(flags_pre);

            b.go();

            if (r.A() != value)
            {
                int a = 10;
            }

            if (flags_post != r.m_AF.m_lo)
            {
                int b = 10;
            }
        }
    }
    return 0;
}