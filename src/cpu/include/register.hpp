#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <cstdint>

struct Flag_Register
{
    bool z; // Zero flag
    bool n; // Subtraction flag (BCD)
    bool h; // Half Carry flag (BCD)
    bool c; // Carry flag
};

typedef union {
    uint16_t u16;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };

} Register_DoubleHalf_u8;

typedef uint8_t Register_u8;
typedef uint16_t Register_u16;

#endif