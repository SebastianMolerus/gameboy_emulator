#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include <vector>
#include <array>

/* DECODER API */

struct Operand
{
    std::string name;
    unsigned bytes;
    bool immediate;
};

struct Opcode
{
    std::string mnemonic;
    unsigned bytes;
    unsigned cycles;
    std::vector<Operand> operands;
    std::array<std::string, 4> flags;
    bool immediate;
};

// Call before any of get_opcode call
bool load_opcodes() noexcept;

bool get_opcode(std::string opcode_hex, Opcode &opcode) noexcept;

#endif