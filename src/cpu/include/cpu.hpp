#ifndef CPU_HPP
#define CPU_HPP

#include "data.hpp"
#include "decoder.hpp"
#include <cstdint>
#include <functional>
#include <span>

constexpr uint16_t VBLANK_ADDR = 0x40;

class Cpu
{
    using function_callback = std::function<void(const CpuData &, const Opcode &)>;
    using interrupt_callback = std::function<void(const CpuData &)>;

  public:
    Cpu(std::span<uint8_t> program, uint16_t vblank_addr = 0x0040);
    void process();
    void after_exec_callback(function_callback callback);
    void vblank_callback(interrupt_callback callback);

  private:
    void vblank();
    void interrupt_check();
    void ime();
    bool fetch_instruction(uint8_t &opcode_hex);
    void exec(Opcode const &op);
    CpuData m_data;
    std::span<uint8_t> m_program;

    function_callback m_callback;
    interrupt_callback m_vblank_cb;
};

#endif
