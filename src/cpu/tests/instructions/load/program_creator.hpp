#include <cstdint>
#include <vector>

struct program_creator
{
    std::vector<uint8_t> m_program;

    program_creator &load_to_BC(uint16_t val);

    program_creator &load_to_DE(uint16_t val);

    program_creator &load_to_HL(uint16_t val);

    program_creator &load_to_SP(uint16_t val);

    program_creator &load_HL_to_SP();
    program_creator &load_HL_to_A();
    program_creator &load_HL_plus_to_A();
    program_creator &load_HL_minus_to_A();

    // if val & 0x80
    // this is substraction from SP
    program_creator &add_to_SP(uint8_t val);

    program_creator &save_SP(uint16_t addr);

    program_creator &push_BC();

    program_creator &push_DE();

    program_creator &push_HL();

    program_creator &push_AF();

    program_creator &pop_BC();

    program_creator &pop_DE();

    program_creator &pop_HL();

    program_creator &pop_AF();

    program_creator &load_B_to_A();
    program_creator &load_C_to_A();
    program_creator &load_C_addr_to_A();
    program_creator &load_D_to_A();
    program_creator &load_E_to_A();
    program_creator &load_H_to_A();
    program_creator &load_L_to_A();

    std::vector<uint8_t> &get();
};