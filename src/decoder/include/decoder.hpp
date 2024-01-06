#ifndef DECODER_HPP
#define DECODER_HPP

#include <algorithm>
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

    size_t operands_size() const
    {
        return std::count_if(m_operands.begin(), m_operands.end(), [](auto &op) { return op.m_name != nullptr; });
    }

    // Z, N, H, C
    std::array<const char *, 4> m_flags;
    bool m_immediate;
};

opcode &get_opcode(uint8_t opcode_hex, bool pref_opcode = false) noexcept;

opcode &get_opcode(std::string_view instruction) noexcept;

#endif