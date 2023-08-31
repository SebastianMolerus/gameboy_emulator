#ifndef CPU_HPP
#define CPU_HPP

#include "decoder.hpp"
#include <cstdint>
#include <functional>
#include <span>

typedef union {
    uint16_t u16;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
} Register_u16;

struct CpuData
{
    Register_u16 AF{0x0000};
    Register_u16 BC{0x0000};
    Register_u16 DE{0x0000};
    Register_u16 HL{0x0000};
    Register_u16 SP{0x0000};
    Register_u16 PC{0x0000};

    uint32_t cycles{0};
};

class Cpu
{
    using function_callback = std::function<void(const CpuData &, const Opcode &)>;

  public:
    Cpu(std::span<uint8_t> program);
    void process();
    void register_function_callback(function_callback callback);

  private:
    bool fetch_instruction(uint8_t &opcode_hex);
    CpuData m_registers;
    function_callback m_callback;
    std::span<uint8_t> m_program;
};

#endif
