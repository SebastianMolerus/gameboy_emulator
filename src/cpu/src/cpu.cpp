#include "cpu.hpp"

void Cpu::reset()
{
    registers.F = {false, false, false, false};
    registers.A = 0;
    registers.BC = 0;
    registers.C = (uint8_t *)&registers.BC;
    registers.B = (((uint8_t *)&registers.BC) + 1);
    registers.DE = 0;
    registers.E = (uint8_t *)&registers.DE;
    registers.D = (((uint8_t *)&registers.DE) + 1);
    registers.HL = 0;
    registers.L = (uint8_t *)&registers.HL;
    registers.H = (((uint8_t *)&registers.HL) + 1);
    registers.PC = 0;
    registers.SP = 0;

    while (stack.size() != 0)
        stack.pop();

    memory.fill(0);
}

void Cpu::clear_flags()
{
    registers.F = {false, false, false, false};
}

uint8_t Cpu::fetch_instruction(uint16_t program_counter)
{
    return memory[program_counter];
}

// ADC - Add Register or Memory to Accumulator With Carry
void Cpu::adc(uint8_t opcode)
{
    // 111 A
    // 000 B
    // 001 C
    // 010 D
    // all E
    // 100 H
    // 101 L

    uint8_t carry_value = registers.F.c ? 1 : 0;
    uint8_t reg_id = opcode & 0x7;

    uint8_t *pReg = nullptr;

    switch (reg_id)
    {
    case 7:
        pReg = &registers.A;
        break;
    case 0:
        pReg = registers.B;
        break;
    case 1:
        pReg = registers.C;
        break;
    case 2:
        pReg = registers.D;
        break;
    case 3:
        pReg = registers.E;
        break;
    case 4:
        pReg = registers.H;
        break;
    case 5:
        pReg = registers.L;
        break;
    default:
        break;
        // memory load pointed by HL
        // pReg = read_adress(registers.HL) //read byte
    }

    uint16_t result16 = registers.A + *pReg + carry_value;
    // if (result16 & 0x100 == 1)
    // {
    //     // SET CARRY FLAG
    //     registers.F.c = true;
    // }
    registers.A = result16 * 0x00FF;
}