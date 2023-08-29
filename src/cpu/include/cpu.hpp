#ifndef CPU_HPP
#define CPU_HPP

#include <array>
#include <cstdint>
#include <stack>

struct Cpu
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
        uint16_t BC{0};
        uint8_t *C = (uint8_t *)&BC;
        uint8_t *B = (((uint8_t *)&BC) + 1);

        // DE
        uint16_t DE{0};
        uint8_t *E = (uint8_t *)&DE;
        uint8_t *D = (((uint8_t *)&DE) + 1);

        // HL
        uint16_t HL{0};
        uint8_t *L = (uint8_t *)&HL;
        uint8_t *H = (((uint8_t *)&HL) + 1);

        // Stack Pointer
        uint16_t SP{0};
        // Program Counter
        uint16_t PC{0};
    };

    Registers registers;
    std::stack<uint8_t> stack;
    std::array<uint8_t, 1024 * 256> memory;

    void reset();
    void clear_flags();
    uint8_t fetch_instruction(uint16_t program_counter);

    // EXAMPLE
    void adc(uint8_t opcode);
};

#endif
