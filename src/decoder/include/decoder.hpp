#ifndef DECODER_HPP
#define DECODER_HPP

#include <array>
#include <cstdint>
#include <string_view>

/* DECODER API */

struct operand
{
    const char *m_name{}; // A, a8, $28, B

    // default value (-1) as "not SET"
    // 1 as TRUE
    // 0 as FALSE
    int8_t m_immediate{-1};
    int8_t m_increment{-1};
    int8_t m_decrement{-1};

    int8_t m_bytes{-1};
};

struct opcode
{
    const char *m_mnemonic{nullptr};
    uint8_t m_bytes{};
    uint8_t m_hex{};
    std::array<uint8_t, 2> m_data{};
    std::array<uint8_t, 2> m_cycles{};
    std::array<operand, 3> m_operands{};
    // Z, N, H, C
    std::array<const char *, 4> m_flags;
    bool m_immediate;
};

// Call before any of get_opcode call
bool load_opcodes() noexcept;

uint8_t get_ld_hex(const char *op1, const char *op2);

opcode &get_opcode(uint8_t opcode_hex) noexcept;
opcode &get_pref_opcode(uint8_t opcode_hex) noexcept;

opcode &get_opcode(std::string_view instruction) noexcept;

#endif