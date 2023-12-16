#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

struct registers;
struct opcode;

class cpu
{
    using cb = std::function<bool(registers const &, opcode const &op, uint8_t)>;

  public:
    // Callback is called after each instruction
    // registers, opcode and wait time for instruction can be viewed
    // cpu working loop ends if callback returns true
    cpu(rw_device &rw_device, cb callback = nullptr);

    ~cpu();

    void start();

  private:
    struct cpu_impl;
    std::unique_ptr<cpu_impl> m_pimpl;
};

#endif
