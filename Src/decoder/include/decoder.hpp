#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include <vector>
#include <array>

/* DECODER API */

struct Operand
{
    const char *name{};
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
    const char *mnemonic;
    uint8_t bytes;
    std::array<uint8_t, 2> cycles{0xFF, 0xFF};
    std::array<Operand, 3> operands;
    std::array<const char *, 4> flags;
    bool immediate;
};

// Call before any of get_opcode call
bool load_opcodes() noexcept;

Opcode &get_opcode(uint8_t opcode_hex) noexcept;

#endif