#ifndef DECODER_HPP
#define DECODER_HPP

#include <array>
#include <cstdint>
#include <string_view>

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
    const char *mnemonic{nullptr};
    uint8_t bytes;
    uint8_t hex;
    std::array<uint8_t, 2> cycles{0xFF, 0xFF};
    std::array<Operand, 3> operands;
    // Z, N, H, C
    std::array<const char *, 4> flags;
    bool immediate;
};

// Call before any of get_opcode call
bool load_opcodes() noexcept;

uint8_t get_ld_hex(const char *op1, const char *op2);

Opcode &get_opcode(uint8_t opcode_hex) noexcept;
Opcode &get_pref_opcode(uint8_t opcode_hex) noexcept;

Opcode &get_opcode(std::string_view instruction) noexcept;

#endif