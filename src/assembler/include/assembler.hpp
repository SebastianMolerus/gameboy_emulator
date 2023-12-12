#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <cstdint>
#include <string_view>
#include <vector>

std::vector<uint8_t> transform(std::string_view assembly);

#endif