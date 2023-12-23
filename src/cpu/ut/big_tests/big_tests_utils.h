#ifndef BIG_TESTS_UTILS_HPP
#define BIG_TESTS_UTILS_HPP

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <json_spirit/json_spirit.h>
#include <sstream>
#include <vector>

namespace big_tests
{
struct data
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

std::vector<data> read_big_data(std::filesystem::path file)
{
    assert(std::filesystem::exists(file));

    std::ifstream big_file(file);
    std::stringstream ss;
    ss << big_file.rdbuf();

    std::vector<data> result;
    for (std::string line; std::getline(ss, line);)
    {
        json_spirit::Value value;
        if (json_spirit::read(line, value))
        {
            data new_data;
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

} // namespace big_tests

#endif