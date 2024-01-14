#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <reg.hpp>
#include <common.hpp>

struct registers;
struct opcode;

class cpu
{
    using cb = std::function<void(registers const &, opcode const &op)>;

  public:
    // 1. Callback is called after each instruction
    // 2. Start_values are used as registers state before run
    cpu(rw_device &rw_device, cb callback = nullptr, registers start_values = {});
    ~cpu();

    void tick();

    struct cpu_impl;

  private:
    std::unique_ptr<cpu_impl> m_pimpl;
};

#endif
