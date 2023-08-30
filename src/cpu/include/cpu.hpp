#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <span>

namespace cpu
{

// For UTs and other components
uint16_t AF();
uint16_t BC();
uint16_t DE();
uint16_t HL();
uint16_t SP();
uint16_t PC();

void process(std::span<uint8_t> program);

void reset();

} // namespace cpu

#endif
