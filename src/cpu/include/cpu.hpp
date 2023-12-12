#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <decoder.hpp>
#include <functional>
#include <reg.hpp>
#include <unordered_map>
#include <variant>

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

class cpu
{
    using cb = std::function<bool(registers const &, opcode const &op, uint8_t)>;

  public:
    // Callback is called after each instruction
    // registers, opcode and wait time for instruction can be viewed
    cpu(rw_device &rw_device, cb callback = nullptr);

    void start();

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
    void reset_all_flags();

  private:
    bool is_carry(uint8_t dest, uint8_t src);
    bool is_half_carry(uint8_t dest, uint8_t src);

    void no_op_defined();

    uint16_t combined_data();

  private:
    uint8_t ld();
    uint8_t LD_HL_SP_e8();
    uint8_t LD_REG16_n16();
    uint8_t LD_Ia16I_SP();
    uint8_t LD_SP_HL();
    uint8_t push();
    uint8_t pop();
    uint8_t LD_REG8_REG8();
    uint8_t LD_IREG16I_A();
    uint8_t LD_REG_n8();
    uint8_t LDH();
    uint8_t LD_REG8_IREG16I();
    uint8_t LD_Ia16I_A();
    uint8_t LD_ICI_A();
    uint8_t LD_IHLI_REG8();

  private:
    uint8_t jmp();
    uint8_t JR_CC_e8();
    uint8_t JR_e8();
    uint8_t JP_nn();
    uint8_t JP_CC_a16();

  private:
    uint8_t arith();
    uint8_t ADD_REG8_REG8();
};

#endif
