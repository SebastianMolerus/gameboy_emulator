#include <decoder.hpp>

#include <json_spirit/json_spirit.h>

#include <filesystem>
#include <fstream>
#include <string_view>

using namespace json_spirit;

namespace
{

std::array<const char *, 57> MNEMONICS_STR{
    "ADC",        "ADD",        "AND",        "CALL",       "CCF",        "CP",         "CPL",        "DAA",
    "DEC",        "DI",         "EI",         "HALT",       "ILLEGAL_D3", "ILLEGAL_DB", "ILLEGAL_DD", "ILLEGAL_E3",
    "ILLEGAL_E4", "ILLEGAL_EB", "ILLEGAL_EC", "ILLEGAL_ED", "ILLEGAL_F4", "ILLEGAL_FC", "ILLEGAL_FD", "INC",
    "JP",         "JR",         "LD",         "LDH",        "NOP",        "OR",         "POP",        "PREFIX",
    "PUSH",       "RET",        "RETI",       "RLA",        "RLCA",       "RRA",        "RRCA",       "RST",
    "SBC",        "SCF",        "STOP",       "SUB",        "XOR",        "BIT",        "CALL",       "RLC",
    "RRC",        "RL",         "SWAP",       "RES",        "SET",        "SRA",        "SRL",        "RR",
    "SLA"};

std::array<const char *, 36> OPERANDS_STR{
    "$00", "$08", "$10", "$18", "$20", "$28", "$30", "$38", "A",   "AF", "B", "BC", "C", "D", "DE", "E", "H", "HL",
    "L",   "NC",  "NZ",  "SP",  "Z",   "a16", "a8",  "e8",  "n16", "n8", "0", "1",  "2", "3", "4",  "5", "6", "7"};

std::array<const char *, 7> FLAGS_STR{"Z", "N", "H", "C", "0", "1", "-"};

const std::filesystem::path JSON_PATH{OPCODES_JSON_PATH};

std::array<opcode, 256> OPCODES_CACHE;
std::array<opcode, 256> PREF_OPCODES_CACHE;

std::vector<std::pair<std::string, opcode>> INSTRUCTIONS;

char const *opcode_MBR_MNEMONIC = "mnemonic";
char const *opcode_MBR_BYTES = "bytes";
char const *opcode_MBR_CYCLES = "cycles";
char const *opcode_MBR_OPERANDS = "operands";
char const *opcode_MBR_IMMEDIATE = "immediate";
char const *opcode_MBR_FLAGS = "flags";

void fill_mnemonic(Pair const &mnemonic, opcode &new_opcode)
{
    std::string_view mnemonic_str = mnemonic.value_.get_str().c_str();
    auto result = std::find(MNEMONICS_STR.begin(), MNEMONICS_STR.end(), mnemonic_str);
    assert(result != MNEMONICS_STR.end());
    new_opcode.m_mnemonic = *result;
}

void fill_bytes(Pair const &bytes, opcode &new_opcode)
{
    new_opcode.m_bytes = bytes.value_.get_int();
}

void fill_cycles(Pair const &cycles, opcode &new_opcode)
{
    Array const arr = cycles.value_.get_array();
    uint8_t idx{};
    for (auto const &val : arr)
    {
        assert(idx < 2);
        new_opcode.m_cycles[idx] = val.get_int();
        ++idx;
    }
}

void fill_operands(Pair const &operands, opcode &new_opcode)
{
    Array const arr = operands.value_.get_array();
    uint8_t idx{};
    for (auto const &object : arr)
    {
        operand new_operand;
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
                new_operand.m_name = *result;
            }
            else if (operand.name_ == "immediate")
            {
                new_operand.m_immediate = operand.value_.get_bool();
            }
            else if (operand.name_ == "bytes")
            {
                new_operand.m_bytes = operand.value_.get_int();
            }
            else if (operand.name_ == "increment")
            {
                new_operand.m_increment = operand.value_.get_bool();
            }
            else if (operand.name_ == "decrement")
            {
                new_operand.m_decrement = operand.value_.get_bool();
            }
        }
        assert(idx < 3);
        new_opcode.m_operands[idx] = new_operand;
        ++idx;
    }
}

void fill_immediate(Pair const &immediate, opcode &new_opcode)
{
    new_opcode.m_immediate = immediate.value_.get_bool();
}

void fill_flags(Pair const &flags, opcode &new_opcode)
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
            new_opcode.m_flags[0] = find_flag(value.get_str());
        else if (name == "N")
            new_opcode.m_flags[1] = find_flag(value.get_str());
        else if (name == "H")
            new_opcode.m_flags[2] = find_flag(value.get_str());
        else if (name == "C")
            new_opcode.m_flags[3] = find_flag(value.get_str());
        else
            assert(false);
    }
}

std::string get_operand(operand const &op)
{
    std::string operand;

    if (!op.m_immediate)
        operand += '[';

    operand += op.m_name;

    if (op.m_increment == 1)
        operand += '+';

    if (op.m_decrement == 1)
        operand += '-';

    if (!op.m_immediate)
        operand += ']';

    return operand;
}

void fill_instruction(opcode const &op)
{
    std::string new_instruction;
    new_instruction += op.m_mnemonic;

    auto const operands_count = std::count_if(op.m_operands.cbegin(), op.m_operands.cend(),
                                              [](operand const &op) { return op.m_name != nullptr; });

    auto add_two_operands = [&new_instruction, &op]() {
        new_instruction += ' ';
        new_instruction += get_operand(op.m_operands[0]);
        new_instruction += ", ";
        new_instruction += get_operand(op.m_operands[1]);
    };

    switch (operands_count)
    {
    case 0:
        break;
    case 1:
        new_instruction += ' ';
        new_instruction += get_operand(op.m_operands[0]);
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
        new_instruction += get_operand(op.m_operands[2]);
        break;
    default:
        assert(false);
    }

    INSTRUCTIONS.push_back(std::make_pair(std::move(new_instruction), op));
}

// {"0x01", Value}
void process_opcode(json_spirit::Pair const &op, std::array<opcode, 256> &cache)
{
    uint8_t const hex = std::stoi(op.name_, 0, 16);
    opcode &new_opcode = cache[hex];
    new_opcode.m_hex = hex;

    Object const &opcode_members = op.value_.get_obj();
    for (auto const &member : opcode_members)
    {
        if (member.name_ == opcode_MBR_MNEMONIC)
            fill_mnemonic(member, new_opcode);
        else if (member.name_ == opcode_MBR_BYTES)
            fill_bytes(member, new_opcode);
        else if (member.name_ == opcode_MBR_CYCLES)
            fill_cycles(member, new_opcode);
        else if (member.name_ == opcode_MBR_OPERANDS)
            fill_operands(member, new_opcode);
        else if (member.name_ == opcode_MBR_IMMEDIATE)
            fill_immediate(member, new_opcode);
        else if (member.name_ == opcode_MBR_FLAGS)
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
    if (OPCODES_CACHE[0].m_mnemonic != nullptr)
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
        if (e.m_bytes != 1)
            continue;

        if (e.m_mnemonic != MNEMONICS_STR[26])
            continue;

        if (strcmp(e.m_operands[0].m_name, op1) == 0 && strcmp(e.m_operands[1].m_name, op2) == 0)
        {
            if (e.m_bytes == 1 && e.m_operands[0].m_immediate == 1 && e.m_operands[1].m_immediate == 1)
                return e.m_hex;
        }
    }
    assert(false);
    return 0;
}

opcode &get_opcode(uint8_t opcode_hex) noexcept
{
    return OPCODES_CACHE[opcode_hex];
}

opcode &get_pref_opcode(uint8_t opcode_hex) noexcept
{
    return PREF_OPCODES_CACHE[opcode_hex];
}

opcode &get_opcode(std::string_view instruction) noexcept
{
    auto result = std::find_if(
        INSTRUCTIONS.begin(), INSTRUCTIONS.end(),
        [&instruction](std::pair<std::string, opcode> &instr_op) { return instr_op.first == instruction; });

    static opcode op;
    if (result == INSTRUCTIONS.cend())
        return op;

    return result->second;
}