#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <decoder.hpp>
#include <functional>
#include <reg.hpp>
#include <unordered_map>
#include <variant>

class cpu;

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

class cpu
{
    using cb = std::function<bool(registers const &, opcode const &op)>;

  public:
    cpu(rw_device &rw_device, cb callback = nullptr);

    void start();

    enum flag : uint8_t
    {
        C = (1 << 3),
        H = (1 << 4),
        N = (1 << 5),
        Z = (1 << 6)
    };

  private:
    registers m_reg;
    rw_device &m_rw_device;
    opcode m_op;

    cb m_callback;

    using processing_func = uint8_t (cpu::*)();
    static const std::unordered_map<const char *, processing_func> m_mapper;

    uint8_t read_byte();

    void set(flag f);
    void reset(flag f);

  private:
    uint8_t ld();
    uint8_t LD_HL_SP_e8();
    uint8_t LD_REG16_n16();
};

#endif
