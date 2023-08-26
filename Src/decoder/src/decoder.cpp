#include <decoder.hpp>

#include <restclient-cpp/restclient.h>
#include <json_spirit/json_spirit.h>

#include <unordered_map>
#include <format>

using namespace json_spirit;
using namespace RestClient;

namespace
{

struct Decoder_exception : public std::exception
{
    using std::exception::exception;
};

void check_type(Value const &val, Value_type expected_type, std::string error_msg)
{
    if (val.type() != expected_type)
        throw Decoder_exception(error_msg.c_str());
}

const char *DIFFERENT_JSON_FORMAT_EXPECTED = "Expected different Json format";

const std::string OPCODE_MBR_MNEMONIC = "mnemonic";
const std::string OPCODE_MBR_BYTES = "bytes";
const std::string OPCODE_MBR_CYCLES = "cycles";
const std::string OPCODE_MBR_OPERANDS = "operands";
const std::string OPCODE_MBR_IMMEDIATE = "immediate";

std::unordered_map<std::string, Opcode> OPCODE_CACHE;

void fill_mnemonic(Pair const &mnemonic, Opcode &new_opcode, std::string const &opcode_hex)
{
    if (mnemonic.value_.type() != Value_type::str_type)
        throw Decoder_exception(std::format("Expected to have 'string' mnemonic for {}", opcode_hex).c_str());
    new_opcode.mnemonic = mnemonic.value_.get_str();
}

void fill_bytes(Pair const &bytes, Opcode &new_opcode, std::string const &opcode_hex)
{
    if (bytes.value_.type() != Value_type::int_type)
        throw Decoder_exception(std::format("Expected to have 'int' bytes for {}", opcode_hex).c_str());
    new_opcode.bytes = bytes.value_.get_int();
}

void fill_cycles(Pair const &cycles, Opcode &new_opcode, std::string const &opcode_hex)
{
    if (cycles.value_.type() != Value_type::array_type)
        throw Decoder_exception(std::format("Expected to have 'array' cycles for {}", opcode_hex).c_str());
    Array const arr = cycles.value_.get_array();

    for (auto const &val : arr)
    {
        if (val.type() != Value_type::int_type)
            throw Decoder_exception(std::format("Expected to have 'int' cycles for {} ", opcode_hex).c_str());
        new_opcode.cycles.push_back(val.get_int());
    }
}

void fill_operands(Pair const &operands, Opcode &new_opcode, std::string const &opcode_hex)
{
    if (operands.value_.type() != Value_type::array_type)
        throw Decoder_exception(std::format("Expected to have 'array' operands for {}", opcode_hex).c_str());

    Array const arr = operands.value_.get_array();
    for (auto const &object : arr)
    {
        if (object.type() != Value_type::obj_type)
            throw Decoder_exception(std::format("Expected to have 'object' inside operand for {}", opcode_hex).c_str());

        Operand new_operand;
        // { name, immediate} , {name, bytes, immediate}
        for (auto const &operand : object.get_obj())
        {
            if (operand.name_ == "name")
            {
                check_type(operand.value_, Value_type::str_type,
                           "Expected that type of operand name is string, opcode =" + opcode_hex);
                new_operand.name = operand.value_.get_str();
            }
            else if (operand.name_ == "immediate")
            {
                check_type(operand.value_, Value_type::bool_type,
                           "Expected that type of operand immediate is bool, opcode =" + opcode_hex);
                new_operand.immediate = operand.value_.get_bool();
            }
            else if (operand.name_ == "bytes")
            {
                check_type(operand.value_, Value_type::int_type,
                           "Expected that type of operand bytes is int, opcode =" + opcode_hex);
                new_operand.bytes = operand.value_.get_int();
            }
            else if (operand.name_ == "increment")
            {
                check_type(operand.value_, Value_type::bool_type,
                           "Expected that type of operand increment is bool, opcode =" + opcode_hex);
                new_operand.increment = operand.value_.get_bool();
            }
            else if (operand.name_ == "decrement")
            {
                check_type(operand.value_, Value_type::bool_type,
                           "Expected that type of operand decrement is bool, opcode =" + opcode_hex);
                new_operand.decrement = operand.value_.get_bool();
            }
            else
                throw Decoder_exception(std::format("Unknown operand for {}", opcode_hex).c_str());
        }
        new_opcode.operands.push_back(std::move(new_operand));
    }
}

void fill_immediate(Pair const &immediate, Opcode &new_opcode, std::string const &opcode_hex)
{
    if (immediate.value_.type() != Value_type::bool_type)
        throw Decoder_exception(std::format("Expected to have 'bool' immediate for {}", opcode_hex).c_str());

    new_opcode.immediate = immediate.value_.get_bool();
}

void process_opcode(json_spirit::Pair const &opcode)
{
    if (opcode.value_.type() != Value_type::obj_type)
        throw Decoder_exception(DIFFERENT_JSON_FORMAT_EXPECTED);

    Opcode new_opcode;

    Object const &opcode_members = opcode.value_.get_obj();
    for (auto const &member : opcode_members)
    {
        if (member.name_ == OPCODE_MBR_MNEMONIC)
            fill_mnemonic(member, new_opcode, opcode.name_);
        else if (member.name_ == OPCODE_MBR_BYTES)
            fill_bytes(member, new_opcode, opcode.name_);
        else if (member.name_ == OPCODE_MBR_CYCLES)
            fill_cycles(member, new_opcode, opcode.name_);
        else if (member.name_ == OPCODE_MBR_OPERANDS)
            fill_operands(member, new_opcode, opcode.name_);
        else if (member.name_ == OPCODE_MBR_IMMEDIATE)
            fill_immediate(member, new_opcode, opcode.name_);
        // FLAGS HERE
    }

    if (auto [_, result] = OPCODE_CACHE.insert_or_assign(opcode.name_, new_opcode); !result)
        throw Decoder_exception("Key already exists");
}

void cache_opcodes()
{
    constexpr int HTTP_RESPONSE_SUCCESS = 200;
    if (Response r = get("https://gbdev.io/gb-opcodes/Opcodes.json"); r.code == HTTP_RESPONSE_SUCCESS)
    {
        std::string const whole_json = r.body;

        Value val;
        if (!read(whole_json, val))
            throw Decoder_exception("Cannot read Json");

        if (val.type() != Value_type::obj_type)
            throw Decoder_exception(DIFFERENT_JSON_FORMAT_EXPECTED);

        Object const main_obj = val.get_obj();

        if (auto const unprefixed_opcodes = main_obj[0].value_; unprefixed_opcodes.type() != Value_type::obj_type)
            throw Decoder_exception(DIFFERENT_JSON_FORMAT_EXPECTED);
        else
        {
            Object const op_codes = unprefixed_opcodes.get_obj();
            for (auto const &oc : op_codes)
                process_opcode(oc);
        }
    }
    else
    {
        throw Decoder_exception(std::format("Cannot connect do URL, http response code {}", r.code).c_str());
    }
}

} // namespace

bool load_opcodes() noexcept
{
    try
    {
        cache_opcodes();
        return true;
    }
    catch (const Decoder_exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    catch (...)
    {
        return false;
    }
}

bool get_opcode(std::string opcode_hex, Opcode &opcode) noexcept
{
    if (!OPCODE_CACHE.contains(opcode_hex))
        return false;

    opcode = OPCODE_CACHE[opcode_hex];
    return true;
}