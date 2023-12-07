#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <decoder.hpp>
#include <functional>
#include <unordered_map>

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

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
};

enum flag : uint8_t
{
    C = (1 << 3),
    H = (1 << 4),
    N = (1 << 5),
    Z = (1 << 6)
};

class cpu
{
    using callback_sig = std::function<bool(registers const &)>;

  public:
    cpu(rw_device &rw_device, callback_sig callback = nullptr);

    void start();

  private:
    registers m_regs;
    rw_device &m_rw_device;
    Opcode m_op;

    callback_sig m_callback;

    using processing_func = uint8_t (cpu::*)();
    static const std::unordered_map<const char *, processing_func> m_mapper;

    uint8_t read_byte();

    void set(flag f);
    void reset(flag f);

    uint16_t &PC();
    uint16_t &SP();
    uint16_t &HL();
    uint16_t &BC();

    uint8_t &A();

    uint8_t ld();
    void LD_HL_SP_e8();

    uint8_t push();
};

#endif
