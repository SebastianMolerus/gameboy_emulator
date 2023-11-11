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
    CpuData();

    Register_u16 AF{0x0000};
    Register_u16 BC{0x0000};
    Register_u16 DE{0x0000};
    Register_u16 HL{0x0000};
    Register_u16 SP{0x0000};
    Register_u16 PC{0x0000};

    uint32_t cycles{0};
    uint16_t *get_word(const char *reg_name);
    uint8_t *get_byte(const char *reg_name);

    enum Flags : uint8_t
    {
        FLAG_C = (1 << 3),
        FLAG_H = (1 << 4),
        FLAG_N = (1 << 5),
        FLAG_Z = (1 << 6)
    };

    bool is_flag_set(Flags flag);
    void set_flag(Flags flag);
    void unset_flag(Flags flag);

    bool m_IME{};

    uint8_t &IE()
    {
        return m_memory[0xFFFF];
    }

    //  Bit 4: Transition from High to Low of Pin number P10-P13
    //  Bit 3: Serial I/O transfer complete
    //  Bit 2: Timer Overflow
    //  Bit 1: LCDC (see STAT)
    //  Bit 0: V-Blank
    uint8_t &IF()
    {
        return m_memory[0xFF0F];
    }

    std::array<uint8_t, 0xFFFF + 1> m_memory{};

    std::unordered_map<const char *, uint16_t *> m_register_map_word;
    std::unordered_map<const char *, uint8_t *> m_register_map_byte;
};

class Cpu
{
    using function_callback = std::function<void(const CpuData &, const Opcode &)>;

  public:
    Cpu(std::span<uint8_t> program, uint16_t vblank_addr = 0x0040);
    void process();
    void register_function_callback(function_callback callback);

  private:
    void push_PC();
    void vblank();
    void interrupt_check();

    bool fetch_instruction(uint8_t &opcode_hex);
    void exec(Opcode const &op);
    CpuData m_data;
    function_callback m_callback;
    std::span<uint8_t> m_program;

    uint16_t m_vblank_addr;
};

#endif
