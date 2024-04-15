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
    virtual uint8_t read(uint16_t addr, device d = device::CPU, bool direct = false) = 0;
    virtual void write(uint16_t addr, uint8_t data, device d = device::CPU, bool direct = false) = 0;
};

struct screen_coordinates
{
    uint8_t m_x{};
    uint8_t m_y{};
};

struct sprite
{
    uint8_t m_y_pos{};
    uint8_t m_x_pos{};
    uint8_t m_tile_index{};
    uint8_t m_flags{};

    uint8_t priority() const
    {
        return checkbit(m_flags, 7) ? 1 : 0;
    }

    uint8_t palette() const
    {
        return checkbit(m_flags, 4) ? 1 : 0;
    }

    uint16_t line_addr(uint8_t line) const
    {
        return 0x8000 + (m_tile_index * 16) + (line * 2);
    }
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