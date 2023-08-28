#include "cpu.hpp"

void Cpu::reset() 
{
    registers.F = {false, false, false, false};
    registers.A = 0;
    registers.BC = 0;
    registers.C  = (uint8_t*)&registers.BC;
    registers.B = (((uint8_t*)&registers.BC) +1);
    registers.DE = 0;
    registers.E  = (uint8_t*)&registers.DE;
    registers.D = (((uint8_t*)&registers.DE) +1);
    registers.HL = 0;
    registers.L  = (uint8_t*)&registers.HL;
    registers.H = (((uint8_t*)&registers.HL) +1);
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