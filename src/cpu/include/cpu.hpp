#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <string_view>
#include <unordered_map>

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

typedef union {
    uint16_t u16;
    struct
    {
        uint8_t lo;
        uint8_t hi;
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
};

class cpu
{
  public:
    cpu(rw_device &rw_device);
    void start();

  private:
    registers m_regs;
    rw_device &m_rw_device;

    const std::unordered_map<std::string_view, uint8_t (cpu::*)()> m_mnemonic_map;

    uint8_t get_next_byte();

    uint8_t ld();
};

#endif
