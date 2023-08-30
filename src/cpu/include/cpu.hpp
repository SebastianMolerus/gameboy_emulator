#ifndef CPU_HPP
#define CPU_HPP

#include "constants.hpp"
#include "register.hpp"
#include <array>
#include <cstdint>
#include <stack>

struct Cpu
{
    Cpu();
    // ACC & Flags
    Register_u8 A;      // Accumulator
    Flag_Register Flag; // Flag register

    // Basic Registers
    Register_DoubleHalf_u8 BC;
    Register_DoubleHalf_u8 DE;
    Register_DoubleHalf_u8 HL;

    // Stack Pointer
    Register_u16 SP;
    // Program Counter
    Register_u16 PC;

    uint32_t cycles;

    std::stack<uint8_t> stack;
    std::array<uint8_t, MEMORY_SIZE> memory;

    void reset();
    void clear_flags();

    void process();

    uint8_t fetch_instruction(const uint16_t program_counter);
};

#endif
