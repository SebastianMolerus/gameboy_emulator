#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include <vector>
#include <array>

/* DECODER API */

struct Operand
{
    std::string name;

    // default value (-1) as "not SET"
    // 1 as TRUE
    // 0 as FALSE
    int8_t bytes{-1};
    int8_t immediate{-1};
    int8_t increment{-1};
    int8_t decrement{-1};
};

struct Opcode
{
    std::string mnemonic;
    unsigned bytes;
    std::vector<unsigned> cycles;
    std::vector<Operand> operands;
    std::array<std::string, 4> flags;
    bool immediate;
};

// Call before any of get_opcode call
bool load_opcodes() noexcept;

bool get_opcode(std::string opcode_hex, Opcode &opcode) noexcept;

#endif