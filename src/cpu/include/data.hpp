#ifndef DATA_HPP
#define DATA_HPP

#include <array>
#include <cstdint>
#include <unordered_map>

typedef union {
    uint16_t u16;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
} Register_u16;

enum class IME_TRANS
{
    ENABLE_AFTER_ONE_INSTRUCTION,
    DISABLE_AFTER_ONE_INSTRUCTION,
    DONT_TOUCH
};

struct CpuData
{
    CpuData();

    Register_u16 AF{0x0000};
    Register_u16 BC{0x0000};
    Register_u16 DE{0x0000};
    Register_u16 HL{0x0000};
    Register_u16 SP{0x0000};
    Register_u16 PC{0x0000};

    uint32_t m_cycles{0};
    IME_TRANS m_ime_trans{IME_TRANS::DONT_TOUCH};
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

    uint8_t &IF()
    {
        return m_memory[0xFF0F];
    }

    std::array<uint8_t, 0xFFFF + 1> m_memory{};

    std::unordered_map<const char *, uint16_t *> m_register_map_word;
    std::unordered_map<const char *, uint8_t *> m_register_map_byte;
};

#endif