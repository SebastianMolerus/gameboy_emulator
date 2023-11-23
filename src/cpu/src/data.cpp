#include "data.hpp"
#include <cassert>
#include <common.hpp>

CpuData::CpuData()
    : m_register_map_word{{OPERANDS_STR[9], &AF.u16},
                          {OPERANDS_STR[11], &BC.u16},
                          {OPERANDS_STR[14], &DE.u16},
                          {OPERANDS_STR[17], &HL.u16},
                          {OPERANDS_STR[21], &SP.u16}},
      m_register_map_byte{{OPERANDS_STR[8], &AF.hi},  {OPERANDS_STR[10], &BC.hi}, {OPERANDS_STR[12], &BC.lo},
                          {OPERANDS_STR[13], &DE.hi}, {OPERANDS_STR[15], &DE.lo}, {OPERANDS_STR[16], &HL.hi},
                          {OPERANDS_STR[18], &HL.lo}}
{
}

uint16_t *CpuData::get_word(const char *reg_name)
{
    assert(m_register_map_word.contains(reg_name));
    return m_register_map_word[reg_name];
}

uint8_t *CpuData::get_byte(const char *reg_name)
{
    assert(m_register_map_byte.contains(reg_name));
    return m_register_map_byte[reg_name];
}

bool CpuData::is_flag_set(Flags flag)
{
    return (0 != (AF.lo & flag));
}

void CpuData::set_flag(Flags flag)
{
    AF.lo |= flag;
}

void CpuData::unset_flag(Flags flag)
{
    AF.lo &= ~flag;
}