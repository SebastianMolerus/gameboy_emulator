#ifndef REG_HPP
#define REG_HPP

#include <cassert>
#include <cstdint>

typedef union {
    uint16_t m_u16;
    struct
    {
        uint8_t m_lo;
        uint8_t m_hi;
    };
} register_u16;

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
    uint16_t &HL()
    {
        return m_HL.m_u16;
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

        assert(false);
    }
};

#endif