#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <cstdint>
#include <string_view>
#include <vector>

std::vector<uint8_t> translate(std::string_view instructions);

#endif