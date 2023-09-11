#include "cpu.hpp"
#include "decoder.hpp"
#include <cassert>
#include <cstring>
#include <variant>

namespace
{

Opcode const *oc;
CpuData *data;
std::span<uint8_t> prog;

void push()
{
    assert(oc->operands[0].name);

    // Do we have enough space
    assert(data->SP.u16 >= 2);

    // decrease stack first
    data->SP.u16 -= 2;

    uint16_t const source_REG = *data->get_word(oc->operands[0].name);
    data->m_memory[data->SP.u16] = static_cast<uint8_t>(source_REG);
    data->m_memory[data->SP.u16 + 1] = source_REG >> 8;
}

void pop()
{
    assert(data->SP.u16 <= 0xFFFD);
    assert(oc->operands[0].name);

    auto target = data->get_word(oc->operands[0].name);
    uint16_t val = data->m_memory[data->SP.u16 + 1]; // MSB
    val <<= 8;
    val |= data->m_memory[data->SP.u16];             // LSB

    *target = val;

    // increase SP after pop
    data->SP.u16 += 2;
}

// 0xF8 : Put SP + n effective address into HL
void LD_HL_SP_e8()
{
    data->unset_flag(CpuData::FLAG_Z);
    data->unset_flag(CpuData::FLAG_N);

    auto const &target = oc->operands[0];
    assert(target.name);

    auto *HL = data->get_word(target.name);
    auto SP = data->SP.u16;

    uint8_t val = prog[1];
    bool minus = val & 0x80;
    val &= 0x7F;

    if (minus)
        SP -= val;
    else
    {
        uint8_t half_carry_check{static_cast<uint8_t>(SP)};
        half_carry_check &= 0xF;
        half_carry_check += (val & 0xF);
        if (half_carry_check & 0x10)
            data->set_flag(CpuData::FLAG_H);

        uint32_t carry_check{SP};
        carry_check += val;
        if (carry_check & 0x10000)
            data->set_flag(CpuData::FLAG_C);

        SP += val;
    }
    *HL = SP;
}

std::variant<uint8_t, uint16_t> get_operand_value(Operand const &operand)
{
    assert(operand.name);

    uint16_t val;
    switch (operand.bytes)
    {
    case 1:
        return prog[1];
    case 2:
        val = prog[2];
        val <<= 8;
        val |= prog[1];
        return val;
    case 0:
        break;
    }

    switch (strlen(operand.name))
    {
    case 1:
        return *data->get_byte(operand.name);
    case 2:
        return *data->get_word(operand.name);
    default:
        assert(false);
    }
}

void save_target(uint8_t source_value)
{
    auto const &target = oc->operands[0];
    assert(target.name != nullptr);

    std::variant<uint8_t, uint16_t> target_value = get_operand_value(target);

    if (!target.immediate)
    {
        if (target_value.index() == 0)
        {
            uint16_t addr{std::get<uint8_t>(target_value)};
            // LD [C], A ( special case )
            if (oc->hex == 0xE2)
                addr |= 0xFF00;
            data->m_memory[addr] = source_value;
        }
        else
        {
            data->m_memory[std::get<uint16_t>(target_value)] = source_value;
        }
        return;
    }

    switch (strlen(target.name))
    {
    case 1:
        *data->get_byte(target.name) = source_value;
        break;
    case 2:
        assert(false);
    default:
        assert(false);
    }
}

void save_target(uint16_t source_value)
{
    auto const &target = oc->operands[0];
    assert(target.name != nullptr);

    std::variant<uint8_t, uint16_t> target_value = get_operand_value(target);

    if (!target.immediate)
    {
        if (target_value.index() == 0)
        {
            data->m_memory[std::get<uint8_t>(target_value)] = source_value;
            data->m_memory[std::get<uint8_t>(target_value) + 1] = source_value >> 8;
        }
        else
        {
            data->m_memory[std::get<uint16_t>(target_value)] = source_value;
            data->m_memory[std::get<uint16_t>(target_value) + 1] = source_value >> 8;
        }
        return;
    }

    switch (strlen(target.name))
    {
    case 1:
        assert(false);
    case 2:
        *data->get_word(target.name) = source_value;
        break;
    default:
        assert(false);
    }
}

void load_source()
{
    auto const &source = oc->operands[1];
    assert(source.name != nullptr);

    std::variant<uint8_t, uint16_t> src_value = get_operand_value(source);

    if (!source.immediate)
    {
        if (src_value.index() == 0)
        {
            uint16_t addr = std::get<uint8_t>(src_value);
            // LD A, [C] ( special case )
            if (oc->hex == 0xF2)
                addr |= 0xFF00;
            src_value = data->m_memory[addr];
        }
        else
        {
            src_value = data->m_memory[std::get<uint16_t>(src_value)];

            if (source.increment == 1)
                *data->get_word(source.name) += 1;

            if (source.decrement == 1)
                *data->get_word(source.name) -= 1;
        }
    }

    if (src_value.index() == 0)
        save_target(std::get<uint8_t>(src_value));
    else if (src_value.index() == 1)
        save_target(std::get<uint16_t>(src_value));
    else
        assert(false);
}

} // namespace

// Main "LD" entry
void load(Opcode const &op, CpuData &cpu_data, std::span<uint8_t> program)
{
    oc = &op;
    data = &cpu_data;
    prog = program;

    switch (op.hex)
    {
    case 0xF8: // add n to SP and copy it to HL
        LD_HL_SP_e8();
        break;
    case 0xF9: // copy HL to SP
    case 0x08: // copy SP value into a16
    case 0x01: // load nn to BC
    case 0x11: // load nn to DE
    case 0x21: // load nn to HL
    case 0x31: // load nn to SP
    case 0x78: // load B to A
    case 0x79: // load C to A
    case 0x7A: // load D to A
    case 0x7B: // load E to A
    case 0x7C: // load H to A
    case 0x7D: // load L to A
    case 0x7E: // load ( HL ) to A
    case 0x7F: // load A to A ( wtf ? )
    case 0x2A: // load ( HL + ) to A
    case 0x3A: // load ( HL - ) to A
    case 0xF2: // Put value at address $FF00 + register C into A.
    case 0x3E: // load n to A
    case 0xFA: // load (a16) to A
    case 0x0A: // load (BC) to A
    case 0x1A: // load (DE) to A
    case 0x06: // load n to B
    case 0x02: // LD [BC], A
    case 0x43:
    case 0xE2:
    case 0x0E:
        load_source();
        break;
    case 0xF1: // pop AF
    case 0xC1: // pop BC
    case 0xD1: // pop DE
    case 0xE1: // pop HL
        pop();
        break;
    case 0xC5: // push BC
    case 0xF5: // push AF
    case 0xD5: // push DE
    case 0xE5: // push HL
        push();
        break;
    default:
        assert(false);
    }
}