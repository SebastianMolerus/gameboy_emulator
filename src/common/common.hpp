#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

enum class device
{
    CPU,
    PPU
};

struct rw_device
{
    virtual ~rw_device() = default;
    virtual uint8_t read(uint16_t addr, device d = device::CPU) = 0;
    virtual void write(uint16_t addr, uint8_t data, device d = device::CPU) = 0;
};

#endif