#include "cpu.hpp"

namespace
{
uint8_t read_byte(const uint16_t address, uint8_t *memory)
{
    return memory[address];
}

uint16_t read_word(const uint16_t address, uint8_t *memory)
{
    uint8_t hi = memory[address + 1];
    uint8_t lo = memory[address];
    return (hi << 8) | lo;
}

void write_byte(const uint16_t address, uint8_t *memory, uint8_t data)
{
    memory[address] = data;
}

void write_word(const uint16_t address, uint8_t *memory, uint8_t data)
{
    uint8_t hi = (data >> 8) & 0xFF;
    uint8_t lo = data & 0xFF;
    memory[address] = lo;
    memory[address + 1] = hi;
}

} // namespace

Cpu::Cpu()
{
    reset();
}

void Cpu::reset()
{
    clear_flags();
    A = 0;
    BC.u16 = 0;
    DE.u16 = 0;
    HL.u16 = 0;
    cycles = 0;
    PC = 0;
    SP = 0;

    while (stack.size() != 0)
        stack.pop();

    memory.fill(0);
}

void Cpu::clear_flags()
{
    Flag.c = Flag.h = Flag.n = Flag.z = false;
}

uint8_t Cpu::fetch_instruction(const uint16_t program_counter)
{
    return memory[program_counter];
}

void Cpu::process()
{
    uint8_t raw_opcode_byte = fetch_instruction(PC);
}
