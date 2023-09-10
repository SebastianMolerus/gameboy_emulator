#include <common.hpp>
#include <decoder.hpp>

#include <json_spirit/json_spirit.h>

#include <filesystem>
#include <fstream>
#include <string_view>

using namespace json_spirit;

namespace
{

const std::filesystem::path JSON_PATH{OPCODES_JSON_PATH};

std::array<Opcode, 256> OPCODES_CACHE;
std::array<Opcode, 256> PREF_OPCODES_CACHE;

std::vector<std::pair<std::string, Opcode>> INSTRUCTIONS;

char const *OPCODE_MBR_MNEMONIC = "mnemonic";
char const *OPCODE_MBR_BYTES = "bytes";
char const *OPCODE_MBR_CYCLES = "cycles";
char const *OPCODE_MBR_OPERANDS = "operands";
char const *OPCODE_MBR_IMMEDIATE = "immediate";
char const *OPCODE_MBR_FLAGS = "flags";

void fill_mnemonic(Pair const &mnemonic, Opcode &new_opcode)
{
    std::string_view mnemonic_str = mnemonic.value_.get_str().c_str();
    auto result = std::find(MNEMONICS_STR.begin(), MNEMONICS_STR.end(), mnemonic_str);
    if (result == MNEMONICS_STR.end())
    {
        int a = 10;
    }
    assert(result != MNEMONICS_STR.end());
    new_opcode.mnemonic = *result;
}

void fill_bytes(Pair const &bytes, Opcode &new_opcode)
{
    new_opcode.bytes = bytes.value_.get_int();
}

void fill_cycles(Pair const &cycles, Opcode &new_opcode)
{
    Array const arr = cycles.value_.get_array();
    uint8_t idx{};
    for (auto const &val : arr)
    {
        assert(idx < 2);
        new_opcode.cycles[idx] = val.get_int();
        ++idx;
    }
}

void fill_operands(Pair const &operands, Opcode &new_opcode)
{
    Array const arr = operands.value_.get_array();
    uint8_t idx{};
    for (auto const &object : arr)
    {
        Operand new_operand;
        // { name, immediate} , {name, bytes, immediate}
        for (auto const &operand : object.get_obj())
        {
            if (operand.name_ == "name")
            {
                std::string_view operand_name = operand.value_.get_str();
                auto result = std::find(OPERANDS_STR.begin(), OPERANDS_STR.end(), operand_name);
                if (result == OPERANDS_STR.end())
                {
                    int a = 10;
                }
                new_operand.name = *result;
            }
            else if (operand.name_ == "immediate")
            {
                new_operand.immediate = operand.value_.get_bool();
            }
            else if (operand.name_ == "bytes")
            {
                new_operand.bytes = operand.value_.get_int();
            }
            else if (operand.name_ == "increment")
            {
                new_operand.increment = operand.value_.get_bool();
            }
            else if (operand.name_ == "decrement")
            {
                new_operand.decrement = operand.value_.get_bool();
            }
        }
        assert(idx < 3);
        new_opcode.operands[idx] = new_operand;
        ++idx;
    }
}

void fill_immediate(Pair const &immediate, Opcode &new_opcode)
{
    new_opcode.immediate = immediate.value_.get_bool();
}

void fill_flags(Pair const &flags, Opcode &new_opcode)
{
    auto find_flag = [](std::string_view to_find) {
        auto result = std::find(FLAGS_STR.begin(), FLAGS_STR.end(), to_find);
        assert(result != FLAGS_STR.end());
        return *result;
    };

    Object const &obj = flags.value_.get_obj();
    for (auto const &[name, value] : obj)
    {
        if (name == "Z")
            new_opcode.flags[0] = find_flag(value.get_str());
        else if (name == "N")
            new_opcode.flags[1] = find_flag(value.get_str());
        else if (name == "H")
            new_opcode.flags[2] = find_flag(value.get_str());
        else if (name == "C")
            new_opcode.flags[3] = find_flag(value.get_str());
        else
            assert(false);
    }
}

std::string get_operand(Operand const &op)
{
    std::string operand;

    if (!op.immediate)
        operand += '[';

    operand += op.name;

    if (op.increment == 1)
        operand += '+';

    if (op.decrement == 1)
        operand += '-';

    if (!op.immediate)
        operand += ']';

    return operand;
}

void fill_instruction(Opcode const &op)
{
    std::string new_instruction;
    new_instruction += op.mnemonic;

    auto const operands_count =
        std::count_if(op.operands.cbegin(), op.operands.cend(), [](Operand const &op) { return op.name != nullptr; });

    auto add_two_operands = [&new_instruction, &op]() {
        new_instruction += ' ';
        new_instruction += get_operand(op.operands[0]);
        new_instruction += ", ";
        new_instruction += get_operand(op.operands[1]);
    };

    switch (operands_count)
    {
    case 0:
        break;
    case 1:
        new_instruction += ' ';
        new_instruction += get_operand(op.operands[0]);
        break;
    case 2:
        add_two_operands();
        break;
    case 3:
        add_two_operands();
        // one special case ( unprefixed : 0xF8 )
        // need to add space before '+'
        if (new_instruction[new_instruction.size() - 1] == '+')
        {
            new_instruction.pop_back();
            new_instruction += " +";
        }
        new_instruction += ' ';
        new_instruction += get_operand(op.operands[2]);
        break;
    default:
        assert(false);
    }

    INSTRUCTIONS.push_back(std::make_pair(std::move(new_instruction), op));
}

// {"0x01", Value}
void process_opcode(json_spirit::Pair const &opcode, std::array<Opcode, 256> &cache)
{
    uint8_t const hex = std::stoi(opcode.name_, 0, 16);
    Opcode &new_opcode = cache[hex];
    new_opcode.hex = hex;

    Object const &opcode_members = opcode.value_.get_obj();
    for (auto const &member : opcode_members)
    {
        if (member.name_ == OPCODE_MBR_MNEMONIC)
            fill_mnemonic(member, new_opcode);
        else if (member.name_ == OPCODE_MBR_BYTES)
            fill_bytes(member, new_opcode);
        else if (member.name_ == OPCODE_MBR_CYCLES)
            fill_cycles(member, new_opcode);
        else if (member.name_ == OPCODE_MBR_OPERANDS)
            fill_operands(member, new_opcode);
        else if (member.name_ == OPCODE_MBR_IMMEDIATE)
            fill_immediate(member, new_opcode);
        else if (member.name_ == OPCODE_MBR_FLAGS)
            fill_flags(member, new_opcode);
        else
            assert(false);
    }

    fill_instruction(new_opcode);
}

bool cache_opcodes()
{
    assert(std::filesystem::exists(JSON_PATH));
    assert(std::filesystem::is_regular_file(JSON_PATH));

    std::ifstream is(JSON_PATH);
    Value value;
    if (!read(is, value))
        return false;

    Object const main_obj = value.get_obj();

    auto const unprefixed_opcodes = main_obj[0].value_;
    Object const up_op_codes = unprefixed_opcodes.get_obj();
    for (auto const &oc : up_op_codes)
        process_opcode(oc, OPCODES_CACHE);

    auto const prefixed_opcodes = main_obj[1].value_;
    Object const p_op_codes = prefixed_opcodes.get_obj();
    for (auto const &oc : p_op_codes)
        process_opcode(oc, PREF_OPCODES_CACHE);

    return true;
}
} // namespace

bool load_opcodes() noexcept
{
    if (OPCODES_CACHE[0].mnemonic != nullptr)
        return true;

    try
    {
        return cache_opcodes();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

uint8_t get_ld_hex(const char *op1, const char *op2)
{
    for (auto &e : OPCODES_CACHE)
    {
        if (e.bytes != 1)
            continue;

        if (e.mnemonic != MNEMONICS_STR[26])
            continue;

        if (strcmp(e.operands[0].name, op1) == 0 && strcmp(e.operands[1].name, op2) == 0)
        {
            if (e.bytes == 1 && e.operands[0].immediate == 1 && e.operands[1].immediate == 1)
                return e.hex;
        }
    }
    assert(false);
    return 0;
}

Opcode &get_opcode(uint8_t opcode_hex) noexcept
{
    return OPCODES_CACHE[opcode_hex];
}

Opcode &get_pref_opcode(uint8_t opcode_hex) noexcept
{
    return PREF_OPCODES_CACHE[opcode_hex];
}

Opcode &get_opcode(std::string_view instruction) noexcept
{
    auto result = std::find_if(
        INSTRUCTIONS.begin(), INSTRUCTIONS.end(),
        [&instruction](std::pair<std::string, Opcode> &instr_op) { return instr_op.first == instruction; });

    static Opcode op;
    if (result == INSTRUCTIONS.cend())
        return op;

    return result->second;
}