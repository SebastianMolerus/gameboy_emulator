#include "cpu.hpp"
#include "decoder.hpp"
#include <cassert>

namespace
{
Opcode const *oc;
CpuData *data;
std::span<uint8_t> prog;

void EI()
{
    data->m_IME = true;
}

void DI()
{
    data->m_IME = false;
}

void NOP()
{
}
} // namespace

void ctrl(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    oc = &op;
    data = &cpu_data;
    prog = program;

    switch (op.hex)
    {
    case 0x00:
        NOP();
        break;
    case 0xF3:
        DI();
        break;
    case 0xFB:
        EI();
        break;
    default:
        assert(false);
    }
}