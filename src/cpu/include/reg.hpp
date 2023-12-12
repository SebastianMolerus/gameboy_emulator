#ifndef REG_HPP
#define REG_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string_view>

typedef union {
    uint16_t m_u16;
    struct
    {
        uint8_t m_lo;
        uint8_t m_hi;
    };
} register_u16;

enum flag : uint8_t
{
    C = (1 << 3),
    H = (1 << 4),
    N = (1 << 5),
    Z = (1 << 6)
};

struct registers
{
    register_u16 m_AF{0x0000};
    register_u16 m_BC{0x0000};
    register_u16 m_DE{0x0000};
    register_u16 m_HL{0x0000};
    register_u16 m_SP{0x0000};
    register_u16 m_PC{0x0000};

    uint16_t &AF()
    {
        return m_AF.m_u16;
    }
    uint8_t &A()
    {
        return m_AF.m_hi;
    }
    uint16_t &BC()
    {
        return m_BC.m_u16;
    }
    uint8_t &B()
    {
        return m_BC.m_hi;
    }
    uint8_t &C()
    {
        return m_BC.m_lo;
    }
    uint16_t &DE()
    {
        return m_DE.m_u16;
    }
    uint8_t &D()
    {
        return m_DE.m_hi;
    }
    uint8_t &E()
    {
        return m_DE.m_lo;
    }
    uint16_t &HL()
    {
        return m_HL.m_u16;
    }
    uint8_t &H()
    {
        return m_HL.m_hi;
    }
    uint8_t &L()
    {
        return m_HL.m_lo;
    }
    uint16_t &SP()
    {
        return m_SP.m_u16;
    }
    uint16_t &PC()
    {
        return m_PC.m_u16;
    }

    uint16_t &get_word(const char *reg)
    {
        if (strcmp(reg, "BC") == 0)
        {
            return BC();
        }
        else if (strcmp(reg, "DE") == 0)
        {
            return DE();
        }
        else if (strcmp(reg, "HL") == 0)
        {
            return HL();
        }
        else if (strcmp(reg, "SP") == 0)
        {
            return SP();
        }
        else if (strcmp(reg, "AF") == 0)
        {
            return AF();
        }

        assert(false);
        static uint16_t ret{0xFFFF};
        return ret;
    }

    uint8_t get_byte(const char *reg) const
    {
        return const_cast<registers *>(this)->get_byte(reg);
    }

    uint8_t &get_byte(const char *reg)
    {
        if (strcmp(reg, "A") == 0)
        {
            return A();
        }
        if (strcmp(reg, "D") == 0)
        {
            return D();
        }
        if (strcmp(reg, "L") == 0)
        {
            return L();
        }
        if (strcmp(reg, "E") == 0)
        {
            return E();
        }
        if (strcmp(reg, "C") == 0)
        {
            return C();
        }
        if (strcmp(reg, "H") == 0)
        {
            return H();
        }
        if (strcmp(reg, "B") == 0)
        {
            return B();
        }
        assert(false);
        static uint8_t ret{0xFF};
        return ret;
    }

    bool is_flag_set(flag flag)
    {
        return (0 != (m_AF.m_lo & flag));
    }

    bool check_condition(std::string_view cond)
    {
        if (cond == "NZ")
            return !is_flag_set(flag::Z);
        else if (cond == "Z")
            return is_flag_set(flag::Z);
        else if (cond == "NC")
            return !is_flag_set(flag::C);
        else if (cond == "C")
            return is_flag_set(flag::C);

        assert(false);
        return false;
    }
};

#endif