#include "translator.hpp"
#include <cassert>
#include <cctype>
#include <cstring>
#include <decoder.hpp>
#include <string>

namespace
{
std::vector<std::string> parse_instructions(std::string_view instructions)
{
    std::vector<std::string> result;

    auto first_char = instructions.find_first_not_of(" \n");
    auto last_char = instructions.find_last_not_of(" \n");

    auto const s = instructions.size();

    instructions.remove_prefix(first_char);
    instructions.remove_suffix(s - 1 - last_char);

    std::string str;
    for (auto iter = instructions.begin(); iter != instructions.end();)
    {
        if (*iter != '\n')
        {
            str += *iter;
            ++iter;
            continue;
        }
        else
        {
            result.push_back(str);
            str.clear();

            while (std::isspace(static_cast<int>(*iter)))
                ++iter;
        }
    }

    if (!str.empty())
        result.push_back(str);

    return result;
}
} // namespace

std::vector<uint8_t> translate(std::string_view instructions)
{
    load_opcodes();

    auto commands = parse_instructions(instructions);

    std::vector<uint8_t> result;
    for (std::string const &cmd : commands)
    {
        size_t const x_pos = cmd.find_first_of('x');
        size_t pos_end{x_pos};

        if (x_pos == std::string::npos)
        {
            result.push_back(get_opcode(cmd).hex);
            continue;
        }

        size_t const zero_pos = x_pos - 1;
        assert(zero_pos > 0);

        char hex[5]{};

        char const *cc = &cmd[x_pos + 1];

        int i{};
        while (std::isalnum(static_cast<int>(*cc)) && i < 4)
        {
            hex[i] = *cc;
            ++i;
            ++cc;
            ++pos_end;
        }

        const uint16_t value = std::stoi(hex, 0, 16);

        auto const old_size = result.size();

        // 16 bit
        if (value & 0xFF00)
        {

            for (auto s : {"n16", "a16"})
            {
                std::string com{cmd};
                auto try_command = com.replace(zero_pos, pos_end - zero_pos + 1, s);
                Opcode op = get_opcode(try_command);
                if (op.hex)
                {
                    result.push_back(op.hex);
                    result.push_back(value);
                    result.push_back(value >> 8);
                    break;
                }
            }
        }
        else
        {
            for (auto s : {"n8", "a8", "e8", "n16", "a16"})
            {
                std::string com{cmd};
                auto try_command = com.replace(zero_pos, pos_end - zero_pos + 1, s);
                Opcode op = get_opcode(try_command);
                if (op.hex)
                {
                    result.push_back(op.hex);
                    result.push_back(value);
                    if (strcmp(s, "n16") == 0 || strcmp(s, "a16") == 0)
                        result.push_back(0x0);
                    break;
                }
            }
        }

        // something was added
        assert(old_size != result.size());
    }

    return result;
}