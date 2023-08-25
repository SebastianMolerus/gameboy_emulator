#include <decoder.hpp>

#include <restclient-cpp/restclient.h>
#include <json_spirit/json_spirit.h>

#include <unordered_map>
#include <format>

// TODO remove when done
// typedef std::vector< string, Value > Object;
// typedef std::vector< Value > Array;

using namespace json_spirit;
using namespace RestClient;

namespace
{

struct Decoder_exception : public std::exception
{
    using std::exception::exception;
};

const char *DIFFERENT_JSON_FORMAT_EXPECTED = "Expected different Json format";

const std::string OPCODE_MBR_MNEMONIC = "mnemonic";

std::unordered_map<std::string, Opcode> OPCODE_CACHE;

void fill_mnemonic(Pair const &mnemonic, Opcode &new_opcode)
{
    if (mnemonic.value_.type() != Value_type::str_type)
        throw Decoder_exception(std::format("Expected to have string mnemonic for {}", mnemonic.name_).c_str());
    new_opcode.mnemonic = mnemonic.value_.get_str();
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
            fill_mnemonic(member, new_opcode);
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