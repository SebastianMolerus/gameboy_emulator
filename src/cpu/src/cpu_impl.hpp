#ifndef CPU_IMPL_HPP
#define CPU_IMPL_HPP

#include <cpu.hpp>
#include <cstdint>
#include <decoder.hpp>
#include <reg.hpp>

struct cpu::cpu_impl
{
    using processing_func = uint8_t (cpu::cpu_impl::*)();

    cpu_impl(rw_device &rw_device, cb callback = nullptr);
    ~cpu_impl() = default;

    static const std::unordered_map<const char *, processing_func> m_mapper;

    registers m_reg;
    rw_device &m_rw_device;
    opcode m_op;
    bool m_IME{}; // Interrupt master enable;
    cb m_callback;

    // working loop entry
    void start();

    // read byte from memory pointed by PC
    // increment PC
    uint8_t read_byte();

    // flags operations
    void set(flag f);
    void reset(flag f);
    void reset_all_flags();

    bool is_carry_on_addition_byte(uint8_t dest, uint8_t src);
    bool is_half_carry_on_addition_byte(uint8_t dest, uint8_t src);
    bool is_carry_on_addition_word(uint16_t dst, uint16_t src);
    bool is_half_carry_on_addition_word(uint16_t dst, uint16_t src);
    bool is_carry_on_substraction_byte(uint8_t dest, uint8_t src);
    bool is_half_carry_on_substraction_byte(uint8_t dest, uint8_t src);

    // throws runtime error with current opcode hex
    void no_op_defined();

    // combine Word from incoming data
    uint16_t combined_data();

    void push_PC();
    void pop_PC();

    // Main entry for Loads
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

    // Main entry for jumps
    uint8_t jmp();
    uint8_t JR_CC_e8();
    uint8_t JR_e8();
    uint8_t JP_nn();
    uint8_t JP_CC_a16();
    uint8_t JP_HL();
    uint8_t CALL_CC_a16();
    uint8_t CALL_a16();
    uint8_t RET();
    uint8_t RET_CC();
    uint8_t RETI();
    uint8_t RST_nn();

    // Main entry for ALU
    uint8_t alu();
    uint8_t ADD_A_REG8();
    uint8_t ADD_A_IHLI();
    uint8_t ADD_A_n8();
    uint8_t ADC_n8();
    uint8_t ADC_A_REG8();
    uint8_t ADC_A_IHLI();
    uint8_t SUB_A_REG8();
    uint8_t SUB_A_IHLI();
    uint8_t SUB_A_n8();
    uint8_t ADD_SP_e8();
    uint8_t ADD_HL_REG16();
    uint8_t SBC_A_REG8();
    uint8_t SBC_A_IHLI();
    uint8_t SBC_A_n8();

    // Main entry for misc
    uint8_t misc();
    uint8_t NOP();
};

#endif
