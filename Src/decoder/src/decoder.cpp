#include <decoder.hpp>

#include <json_spirit/json_spirit.h>

#include <filesystem>
#include <fstream>
#include <string_view>

using namespace json_spirit;
using namespace RestClient;

namespace
{

const std::filesystem::path JSON_PATH{OPCODES_JSON_PATH};

constexpr std::array<const char *, 45> MNEMONICS_STR{
    "ADC",        "ADD",        "AND",        "CALL",       "CCF",        "CP",         "CPL",        "DAA",
    "DEC",        "DI",         "EI",         "HALT",       "ILLEGAL_D3", "ILLEGAL_DB", "ILLEGAL_DD", "ILLEGAL_E3",
    "ILLEGAL_E4", "ILLEGAL_EB", "ILLEGAL_EC", "ILLEGAL_ED", "ILLEGAL_F4", "ILLEGAL_FC", "ILLEGAL_FD", "INC",
    "JP",         "JR",         "LD",         "LDH",        "NOP",        "OR",         "POP",        "PREFIX",
    "PUSH",       "RET",        "RETI",       "RLA",        "RLCA",       "RRA",        "RRCA",       "RST",
    "SBC",        "SCF",        "STOP",       "SUB",        "XOR"};

constexpr std::array<const char *, 28> OPERANDS_STR{"$00", "$08", "$10", "$18", "$20", "$28", "$30", "$38", "A", "AF",
                                                    "B",   "BC",  "C",   "D",   "DE",  "E",   "H",   "HL",  "L", "NC",
                                                    "NZ",  "SP",  "Z",   "a16", "a8",  "e8",  "n16", "n8"};

std::array<Opcode, 256> OPCODES_CACHE;

char const *OPCODE_MBR_MNEMONIC = "mnemonic";
char const *OPCODE_MBR_BYTES = "bytes";
char const *OPCODE_MBR_CYCLES = "cycles";
char const *OPCODE_MBR_OPERANDS = "operands";
char const *OPCODE_MBR_IMMEDIATE = "immediate";

void fill_mnemonic(Pair const &mnemonic, Opcode &new_opcode)
{
    std::string_view mnemonic_str = mnemonic.value_.get_str().c_str();
    auto result = std::find(MNEMONICS_STR.begin(), MNEMONICS_STR.end(), mnemonic_str);
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
                assert(result != OPERANDS_STR.end());
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

// {"0x01", Value}
void process_opcode(json_spirit::Pair const &opcode)
{
    static uint8_t hex{0x00};
    Opcode &new_opcode = OPCODES_CACHE[hex];

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
        // FLAGS HERE
    }
    ++hex;
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

    Object const op_codes = unprefixed_opcodes.get_obj();
    for (auto const &oc : op_codes)
        process_opcode(oc);

    return true;
}

} // namespace

bool load_opcodes() noexcept
{
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

Opcode &get_opcode(uint8_t opcode_hex) noexcept
{
    return OPCODES_CACHE[opcode_hex];
}