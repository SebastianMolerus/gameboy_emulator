#ifndef CPU_IMPL_HPP
#define CPU_IMPL_HPP

#include <cpu.hpp>
#include <cstdint>
#include <decoder.hpp>
#include <reg.hpp>
#include <sstream>
#include <stdexcept>

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
    bool m_pref{};
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
    void no_op_defined(std::string module_name = {});

    // combine Word from incoming data
    uint16_t combined_data();

    void push_PC();
    void pop_PC();

    // Main entry for Loads
    uint8_t ld();
    void LD_HL_SP_e8();
    void LD_REG16_n16();
    void LD_Ia16I_SP();
    void LD_SP_HL();
    void push();
    void pop();
    void LD_REG8_REG8();
    void LD_IREG16I_A();
    void LD_REG_n8();
    void LDH();
    void LD_REG8_IREG16I();
    void LD_Ia16I_A();
    void LD_ICI_A();
    void LD_IHLI_REG8();

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
    void ADD_A_REG8();
    void ADD_A_IHLI();
    void ADD_A_n8();
    void ADC_n8();
    void ADC_A_REG8();
    void ADC_A_IHLI();
    void SUB_A_REG8();
    void SUB_A_IHLI();
    void SUB_A_n8();
    void ADD_SP_e8();
    void ADD_HL_REG16();
    void SBC_A_REG8();
    void SBC_A_IHLI();
    void SBC_A_n8();
    void AND_A_REG8();
    void AND_A_IHLI();
    void AND_A_n8();
    void XOR_A_REG8();
    void XOR_A_IHLI();
    void XOR_A_n8();
    void OR_A_REG8();
    void OR_A_IHLI();
    void OR_A_n8();
    void CP_A_REG8();
    void CP_A_IHLI();
    void CP_A_n8();
    void INC_REG16();
    void DEC_REG16();
    void INC_REG8();
    void DEC_REG8();
    void INC_IHLI();
    void DEC_IHLI();
    void DAA();
    void SCF();
    void CPL();
    void CCF();

    // Main entry for shift, rotate, bit
    uint8_t srb();
    void RLCA();
    void RLA();
    void RRCA();
    void RRA();

    // Main entry for shift, rotate, bit ( PREFIXED )
    uint8_t pref_srb();
    void RLC_REG8();
    void RLC_IHLI();
    void RRC_REG8();
    void RRC_IHLI();
    void RL_REG8();
    void RL_IHLI();
    void RR_REG8();
    void RR_IHLI();
    void SLA_REG8();
    void SLA_IHLI();
    void SRA_REG8();
    void SRA_IHLI();
    void SWAP_REG8();
    void SWAP_IHLI();

    // Main entry for misc
    uint8_t misc();
    void NOP();
};

#endif
