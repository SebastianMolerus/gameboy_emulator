#include <cstdint>
#include <decoder.hpp>
#include <string_view>
#include <vector>

struct program_creator
{
    std::vector<uint8_t> m_program;

    program_creator &ld_BC_nn(uint16_t nn);        // 0x01
    program_creator &ld_DE_nn(uint16_t nn);        // 0x11
    program_creator &ld_HL_nn(uint16_t nn);        // 0x21
    program_creator &ld_SP_nn(uint16_t nn);        // 0x31

    program_creator &ld_Ia16I_SP(uint16_t a16);    // 0x08
    program_creator &ld_HL_SP_plus_n8(uint8_t n8); // 0xF8
    program_creator &ld_SP_HL();                   // 0xF9

    program_creator &push_BC();                    // 0xC5
    program_creator &push_DE();                    // 0xD5
    program_creator &push_HL();                    // 0xE5
    program_creator &push_AF();                    // 0xF5

    program_creator &pop_BC();                     // 0xC1
    program_creator &pop_DE();                     // 0xD1
    program_creator &pop_HL();                     // 0xE1
    program_creator &pop_AF();                     // 0xF1

    program_creator &ldh_Ia8I_A(uint8_t a8);       // 0xE0
    program_creator &ldh_A_Ia8I(uint8_t a8);       // 0xF0

    program_creator &ld_ICI_A();                   // 0xE2
    program_creator &ld_A_ICI();                   // 0xF2

    program_creator &ld_Ia16I_A(uint16_t a16);     // 0xEA
    program_creator &ld_A_Ia16I(uint16_t a16);     // 0xFA

    program_creator &ld_IBCI_A();                  // 0x02
    program_creator &ld_IDEI_A();                  // 0x12
    program_creator &ld_A_IBCI();                  // 0x0A
    program_creator &ld_A_IDEI();                  // 0x1A

    program_creator &ld_IHL_plusI_A();             // 0x22
    program_creator &ld_IHL_minusI_A();            // 0x32
    program_creator &ld_A_IHL_plusI();             // 0x2A
    program_creator &ld_A_IHL_minusI();            // 0x3A

    program_creator &ld_B_n8(uint8_t n8);          // 0x06
    program_creator &ld_D_n8(uint8_t n8);          // 0x16
    program_creator &ld_H_n8(uint8_t n8);          // 0x26
    program_creator &ld_IHLI_n8(uint8_t n8);       // 0x36
    program_creator &ld_C_n8(uint8_t n8);          // 0x0E
    program_creator &ld_E_n8(uint8_t n8);          // 0x1E
    program_creator &ld_L_n8(uint8_t n8);          // 0x2E
    program_creator &ld_A_n8(uint8_t n8);          // 0x3E

    program_creator &ld_B_IHLI();                  // 0x46
    program_creator &ld_D_IHLI();                  // 0x56
    program_creator &ld_H_IHLI();                  // 0x66
    program_creator &ld_C_IHLI();                  // 0x4E
    program_creator &ld_E_IHLI();                  // 0x5E
    program_creator &ld_L_IHLI();                  // 0x6E
    program_creator &ld_A_IHLI();                  // 0x7E

    program_creator &ld_IHLI_B();                  // 0x70
    program_creator &ld_IHLI_C();                  // 0x71
    program_creator &ld_IHLI_D();                  // 0x72
    program_creator &ld_IHLI_E();                  // 0x73
    program_creator &ld_IHLI_H();                  // 0x74
    program_creator &ld_IHLI_L();                  // 0x75
    program_creator &ld_IHLI_A();                  // 0x77

    program_creator &ld_reg8_reg8(std::string_view target, std::string_view source);

    std::vector<uint8_t> &get();
};