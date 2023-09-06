#include "program_creator.hpp"
#include <cassert>
#include <cstdint>
#include <vector>

program_creator &program_creator::ld_BC_nn(uint16_t nn)
{
    m_program.push_back(0x01);
    m_program.push_back(nn);
    m_program.push_back(nn >> 8);
    return *this;
}

program_creator &program_creator::ld_DE_nn(uint16_t nn)
{
    m_program.push_back(0x11);
    m_program.push_back(nn);
    m_program.push_back(nn >> 8);
    return *this;
}

program_creator &program_creator::ld_HL_nn(uint16_t nn)
{
    m_program.push_back(0x21);
    m_program.push_back(nn);
    m_program.push_back(nn >> 8);
    return *this;
}

program_creator &program_creator::ld_SP_nn(uint16_t nn)
{
    m_program.push_back(0x31);
    m_program.push_back(nn);
    m_program.push_back(nn >> 8);
    return *this;
}

program_creator &program_creator::ld_Ia16I_SP(uint16_t a16)
{
    m_program.push_back(0x08);
    m_program.push_back(static_cast<uint8_t>(a16));
    m_program.push_back(a16 >> 8);
    return *this;
}

program_creator &program_creator::ld_HL_SP_plus_n8(uint8_t n8)
{
    m_program.push_back(0xF8);
    m_program.push_back(n8);
    return *this;
}

program_creator &program_creator::ld_SP_HL()
{
    m_program.push_back(0xF9);
    return *this;
}

program_creator &program_creator::push_BC()
{
    m_program.push_back(0xC5);
    return *this;
}

program_creator &program_creator::push_DE()
{
    m_program.push_back(0xD5);
    return *this;
}

program_creator &program_creator::push_HL()
{
    m_program.push_back(0xE5);
    return *this;
}

program_creator &program_creator::push_AF()
{
    m_program.push_back(0xF5);
    return *this;
}

program_creator &program_creator::pop_BC()
{
    m_program.push_back(0xC1);
    return *this;
}

program_creator &program_creator::pop_DE()
{
    m_program.push_back(0xD1);
    return *this;
}

program_creator &program_creator::pop_HL()
{
    m_program.push_back(0xE1);
    return *this;
}

program_creator &program_creator::pop_AF()
{
    m_program.push_back(0xF1);
    return *this;
}

program_creator &program_creator::ldh_Ia8I_A(uint8_t a8)
{
    m_program.push_back(0xE0);
    m_program.push_back(a8);
    return *this;
}

program_creator &program_creator::ldh_A_Ia8I(uint8_t a8)
{
    m_program.push_back(0xF0);
    m_program.push_back(a8);
    return *this;
}

program_creator &program_creator::ld_ICI_A()
{
    m_program.push_back(0xE2);
    return *this;
}

program_creator &program_creator::ld_A_ICI()
{
    m_program.push_back(0xF2);
    return *this;
}

program_creator &program_creator::ld_Ia16I_A(uint16_t a16)
{
    m_program.push_back(0xEA);
    m_program.push_back(static_cast<uint8_t>(a16));
    m_program.push_back(static_cast<uint8_t>(a16 >> 8));
    return *this;
}

program_creator &program_creator::ld_A_Ia16I(uint16_t a16)
{
    m_program.push_back(0xFA);
    m_program.push_back(static_cast<uint8_t>(a16));
    m_program.push_back(static_cast<uint8_t>(a16 >> 8));
    return *this;
}

program_creator &program_creator::ld_IBCI_A()
{
    m_program.push_back(0x02);
    return *this;
}

program_creator &program_creator::ld_IDEI_A()
{
    m_program.push_back(0x12);
    return *this;
}

program_creator &program_creator::ld_A_IBCI()
{
    m_program.push_back(0x0A);
    return *this;
}

program_creator &program_creator::ld_A_IDEI()
{
    m_program.push_back(0x1A);
    return *this;
}

program_creator &program_creator::ld_IHL_plusI_A()
{
    m_program.push_back(0x22);
    return *this;
}

program_creator &program_creator::ld_IHL_minusI_A()
{
    m_program.push_back(0x32);
    return *this;
}

program_creator &program_creator::ld_A_IHL_plusI()
{
    m_program.push_back(0x2A);
    return *this;
}

program_creator &program_creator::ld_A_IHL_minusI()
{
    m_program.push_back(0x3A);
    return *this;
}

program_creator &program_creator::ld_B_n8(uint8_t n)
{
    m_program.push_back(0x06);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_D_n8(uint8_t n)
{
    m_program.push_back(0x16);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_H_n8(uint8_t n)
{
    m_program.push_back(0x26);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_IHLI_n8(uint8_t n)
{
    m_program.push_back(0x36);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_C_n8(uint8_t n)
{
    m_program.push_back(0x0E);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_E_n8(uint8_t n)
{
    m_program.push_back(0x1E);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_L_n8(uint8_t n)
{
    m_program.push_back(0x2E);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_A_n8(uint8_t n)
{
    m_program.push_back(0x3E);
    m_program.push_back(n);
    return *this;
}

program_creator &program_creator::ld_B_IHLI()
{
    m_program.push_back(0x46);
    return *this;
}

program_creator &program_creator::ld_D_IHLI()
{
    m_program.push_back(0x56);
    return *this;
}

program_creator &program_creator::ld_H_IHLI()
{
    m_program.push_back(0x66);
    return *this;
}

program_creator &program_creator::ld_C_IHLI()
{
    m_program.push_back(0x4E);
    return *this;
}

program_creator &program_creator::ld_E_IHLI()
{
    m_program.push_back(0x5E);
    return *this;
}

program_creator &program_creator::ld_L_IHLI()
{
    m_program.push_back(0x6E);
    return *this;
}

program_creator &program_creator::ld_A_IHLI()
{
    m_program.push_back(0x7E);
    return *this;
}

program_creator &program_creator::ld_reg8_reg8(std::string_view target, std::string_view source)
{
    static bool res{load_opcodes()};
    assert(res);
    m_program.push_back(get_ld_hex(target.data(), source.data()));
    return *this;
}

std::vector<uint8_t> &program_creator::get()
{
    return m_program;
}
