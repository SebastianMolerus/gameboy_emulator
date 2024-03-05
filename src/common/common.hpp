#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

#define checkbit(byte, nbit) ((byte) & (1 << (nbit)))
#define setbit(byte, nbit) ((byte) |= (1 << (nbit)))
#define clearbit(byte, nbit) ((byte) &= ~(1 << (nbit)))

constexpr uint16_t INTERRUPT_FLAG{0xFF0F};
constexpr uint16_t LCD_Y_COORDINATE{0xFF44};

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

struct sprite
{
    uint8_t y_pos{};
    uint8_t x_pos{};
    uint8_t tile_index{};
    uint8_t flags{};
};

struct color
{
    float R{}, G{}, B{};
};

struct drawing_device
{
    virtual ~drawing_device() = default;
    virtual void after_frame() = 0;
    virtual void push_pixel(color c) = 0;
};

#endif