#include <algorithm>
#include <assembler.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <cassert>
#include <cctype>
#include <cstring>
#include <decoder.hpp>
#include <string>

namespace
{

constexpr char COMMENT_MARK = ';';

std::vector<std::string> parse_instructions(std::string_view instructions)
{
    std::vector<std::string> result;

    std::string line;
    for (auto &cc : instructions)
    {
        if (cc != '\n')
            line.push_back(cc);
        else
        {
            if (line.size())
            {
                // Remove comments after ';'
                auto iter = find(line.begin(), line.end(), COMMENT_MARK);
                if (iter != line.end())
                    line.erase(iter, line.end());

                boost::algorithm::trim(line);
                result.push_back(line);
                line.clear();
            }
        }
    }
    boost::algorithm::trim(line);
    if (line.size())
        result.push_back(line);

    return result;
}
} // namespace

std::vector<uint8_t> transform(std::string_view instructions)
{
    auto commands = parse_instructions(instructions);

    std::vector<uint8_t> result;
    for (std::string const &cmd : commands)
    {
        size_t const x_pos = cmd.find_first_of('x');
        size_t pos_end{x_pos};

        if (x_pos == std::string::npos)
        {
            result.push_back(get_opcode(cmd).m_hex);
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

        // 16 bit
        if (value & 0xFF00)
        {
            for (auto s : {"n16", "a16"})
            {
                std::string com{cmd};
                auto try_command = com.replace(zero_pos, pos_end - zero_pos + 1, s);
                opcode op = get_opcode(try_command);
                if (op.m_hex)
                {
                    result.push_back(op.m_hex);
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
                opcode op = get_opcode(try_command);
                if (op.m_hex)
                {
                    result.push_back(op.m_hex);
                    result.push_back(value);
                    if (strcmp(s, "n16") == 0 || strcmp(s, "a16") == 0)
                        result.push_back(0x0);
                    break;
                }
            }
        }
    }

    return result;
}