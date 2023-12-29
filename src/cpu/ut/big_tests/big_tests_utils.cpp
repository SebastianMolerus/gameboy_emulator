#include "big_tests_utils.h"

#include <json_spirit/json_spirit.h>

namespace
{

std::filesystem::path const test_data_dir{BIG_TEST_DATA_DIR};

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
} // namespace

void validate_cpu_states(std::vector<cpu_data> const &states)
{
    if (states.empty())
        throw std::runtime_error("No data to validate\n");

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
            std::cerr << "Exception during execution of " << data.name << ": " << err.what() << "\n";
            ASSERT_TRUE(false);
        }

        ASSERT_EQ(r.A(), data.final.cpu.a) << "Instruction " << data.name << ". Expected A==" << (int)data.final.cpu.a
                                           << ". Get A==" << (int)r.A() << "\n";
        ASSERT_EQ(r.B(), data.final.cpu.b) << "Instruction " << data.name << ". Expected B==" << (int)data.final.cpu.b
                                           << ". Get B==" << (int)r.B() << "\n";
        ASSERT_EQ(r.C(), data.final.cpu.c) << "Instruction " << data.name << ". Expected C==" << (int)data.final.cpu.c
                                           << ". Get C==" << (int)r.C() << "\n";
        ASSERT_EQ(r.D(), data.final.cpu.d) << "Instruction " << data.name << ". Expected D==" << (int)data.final.cpu.d
                                           << ". Get D==" << (int)r.D() << "\n";
        ASSERT_EQ(r.F(), data.final.cpu.f) << "Instruction " << data.name << ". Expected F==" << (int)data.final.cpu.f
                                           << ". Get F==" << (int)r.F() << "\n";
        ASSERT_EQ(r.E(), data.final.cpu.e) << "Instruction " << data.name << ". Expected E==" << (int)data.final.cpu.e
                                           << ". Get E==" << (int)r.E() << "\n";
        ASSERT_EQ(r.H(), data.final.cpu.h) << "Instruction " << data.name << ". Expected H==" << (int)data.final.cpu.h
                                           << ". Get H==" << (int)r.H() << "\n";
        ASSERT_EQ(r.L(), data.final.cpu.l) << "Instruction " << data.name << ". Expected L==" << (int)data.final.cpu.l
                                           << ". Get L==" << (int)r.L() << "\n";
        ASSERT_EQ(r.PC(), data.final.cpu.pc)
            << "Instruction " << data.name << ". Expected PC==" << (int)data.final.cpu.pc << ". Get PC==" << (int)r.PC()
            << "\n";
        ASSERT_EQ(r.SP(), data.final.cpu.sp)
            << "Instruction " << data.name << ". Expected SP==" << (int)data.final.cpu.sp << ". Get SP==" << (int)r.SP()
            << "\n";

        for (auto const &[addr, value] : data.final.ram)
            ASSERT_EQ(b.m_ram[addr], value);

        ASSERT_EQ(c, data.cycles);
    }
}

std::vector<cpu_data> read_cpu_data(std::filesystem::path file)
{
    if (!std::filesystem::exists(file))
        throw std::runtime_error(std::string{"File doesn't exist: "} + file.string());

    std::ifstream big_file(file);
    std::stringstream ss;
    ss << big_file.rdbuf();
    std::vector<cpu_data> result;

    json_spirit::Value value;
    if (!read(ss, value))
        throw std::runtime_error(std::string{"Cannot read Json from: "} + file.string());

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

    if (result.empty())
        throw std::runtime_error(std::string{"No cpu test data were loaded from file: "} + file.string());

    return result;
}

std::vector<alu_data> read_alu_data(std::string alu_file_name)
{
    std::filesystem::path const file_path{test_data_dir / "alu_tests" / "v1" / alu_file_name};
    if (!std::filesystem::exists(file_path))
        throw std::runtime_error(std::string{"File doesn't exist: "} + file_path.string());

    std::ifstream big_file(file_path);
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

    if (result.empty())
        throw std::runtime_error(std::string{"No alu test data were loaded from file: "} + file_path.string());

    return result;
}

void validate_opcode(uint8_t hex)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(hex);
    std::cout << "Testing Opcode [0x" << ss.str() << "]\n";
    auto file = ss.str() + ".json";
    const std::filesystem::path test_path{test_data_dir / "cpu_tests" / "v1" / file.c_str()};
    auto const all_data = read_cpu_data(test_path);
    validate_cpu_states(all_data);
}

void validate_prefixed(std::string alu_file_name, uint8_t const first_hex)
{
    auto const result = read_alu_data(alu_file_name);

    std::vector<uint8_t> hex{first_hex};

    for (uint8_t i = 1; i < 8; ++i)
        hex.push_back(first_hex + i);

    std::stringstream ss;
    ss << "[Validate prefixed opcodes";
    for (int i : hex)
        ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << i << " ";
    ss << "] \n";
    std::cout << ss.str();

    for (auto const &data : result)
    {
        {
            registers startup;
            startup.B() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[0]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.B(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.C() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[1]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.C(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.D() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[2]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.D(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.E() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[3]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.E(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.H() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[4]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.H(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.L() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[5]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.L(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.HL() = 0x678;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[6]};
            bus b{opcodes, startup};
            b.m_ram[0x678] = data.x;
            b.go();

            ASSERT_EQ(b.m_ram[0x678], data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }

        {
            registers startup;
            startup.A() = data.x;
            startup.F() = data.flags;

            std::array<uint8_t, 2> opcodes{0xCB, hex[7]};
            bus b{opcodes, startup};
            b.go();

            ASSERT_EQ(r.A(), data.result.value);
            ASSERT_EQ(r.F(), data.result.flags);
        }
    }
}
