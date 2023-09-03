#include "program_creator.hpp"
#include <cstdint>
#include <vector>

program_creator &program_creator::load_to_B(uint8_t val)
{
    m_program.push_back(0x01);
    m_program.push_back(0x00);
    m_program.push_back(val);
    return *this;
}

program_creator &program_creator::load_to_C(uint8_t val)
{
    m_program.push_back(0x01);
    m_program.push_back(val);
    m_program.push_back(0x00);
    return *this;
}

program_creator &program_creator::load_to_BC(uint16_t val)
{
    m_program.push_back(0x01);
    m_program.push_back(val);
    m_program.push_back(val >> 8);
    return *this;
}

program_creator &program_creator::load_to_D(uint8_t val)
{
    m_program.push_back(0x11);
    m_program.push_back(0x00);
    m_program.push_back(val);
    return *this;
}

program_creator &program_creator::load_to_E(uint8_t val)
{
    m_program.push_back(0x11);
    m_program.push_back(val);
    m_program.push_back(0x00);
    return *this;
}

program_creator &program_creator::load_to_DE(uint16_t val)
{
    m_program.push_back(0x11);
    m_program.push_back(val);
    m_program.push_back(val >> 8);
    return *this;
}

program_creator &program_creator::load_to_H(uint8_t val)
{
    m_program.push_back(0x21);
    m_program.push_back(0x00);
    m_program.push_back(val);
    return *this;
}

program_creator &program_creator::load_to_L(uint8_t val)
{
    m_program.push_back(0x21);
    m_program.push_back(val);
    m_program.push_back(0x00);
    return *this;
}

program_creator &program_creator::load_to_HL(uint16_t val)
{
    m_program.push_back(0x21);
    m_program.push_back(val);
    m_program.push_back(val >> 8);
    return *this;
}

program_creator &program_creator::load_to_SP(uint16_t val)
{
    m_program.push_back(0x31);
    m_program.push_back(val);
    m_program.push_back(val >> 8);
    return *this;
}

program_creator &program_creator::load_HL_to_SP()
{
    m_program.push_back(0xF9);
    return *this;
}

// if val & 0x80
// this is substraction from SP
program_creator &program_creator::add_to_SP(uint8_t val)
{
    m_program.push_back(0xF8);
    m_program.push_back(val);
    return *this;
}

program_creator &program_creator::save_SP(uint16_t addr)
{
    m_program.push_back(0x08);
    m_program.push_back(static_cast<uint8_t>(addr));
    m_program.push_back(addr >> 8);
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

program_creator &program_creator::load_B_to_A()
{
    m_program.push_back(0x78);
    return *this;
}
program_creator &program_creator::load_C_to_A()
{
    m_program.push_back(0x79);
    return *this;
}
program_creator &program_creator::load_D_to_A()
{
    m_program.push_back(0x7A);
    return *this;
}
program_creator &program_creator::load_E_to_A()
{
    m_program.push_back(0x7B);
    return *this;
}
program_creator &program_creator::load_H_to_A()
{
    m_program.push_back(0x7C);
    return *this;
}
program_creator &program_creator::load_L_to_A()
{
    m_program.push_back(0x7D);
    return *this;
}

std::vector<uint8_t> &program_creator::get()
{
    return m_program;
}
