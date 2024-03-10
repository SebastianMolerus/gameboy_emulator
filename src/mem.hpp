#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <common.hpp>
#include <array>

class memory : public rw_device
{
  public:
    memory();
    uint8_t read(uint16_t addr, device d = device::CPU) override;
    void write(uint16_t addr, uint8_t data, device d = device::CPU) override;

  private:
    bool boot{true};
    std::array<uint8_t, 0xFFFF + 1> whole_memory{};
    std::array<uint8_t, 0xFF + 1> boot_rom{};
};

#endif