#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>

// https://gbdev.io/pandocs/CPU_Comparison_with_Z80.html
class Cpu
{
    struct Flag
    {
        bool z{false}; // Zero flag
        bool n{false}; // Subtraction flag (BCD)
        bool h{false}; // Half Carry flag (BCD)
        bool c{false}; // Carry flag
    };

    struct Registers
    {
        // ACC & Flags
        uint8_t A{0}; // Accumulator
        Flag F{};     // Flag register
        // BC
        uint8_t B{0};
        uint8_t C{0};
        // DE
        uint8_t D{0};
        uint8_t E{0};
        // HL
        uint8_t H{0};
        uint8_t L{0};
        // Stack Pointer
        uint16_t SP{0};
        // Program Counter
        uint16_t PC{0};
    };

  public:
    void fetch();
    void decode();
    void excute();
    
    Registers registers;
};

#endif
