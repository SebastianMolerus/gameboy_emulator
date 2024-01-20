#ifndef PPU_IMPL_HPP
#define PPU_IMPL_HPP

#include <ppu.hpp>
#include <array>

std::array<uint8_t, 8> convert_line_to_ids(uint16_t line);

struct ppu::ppu_impl
{
    ppu_impl(rw_device &rw_device, drawing_device &drawing_device);
    rw_device &m_rw_device;
    drawing_device &m_drawing_device;
    void dot();
    void draw();

    enum class STATE
    {
        OAM_SCAN,
        DRAWING_PIXELS,
        HORIZONTAL_BLANK,
        VERTICAL_BLANK
    };

    STATE m_current_state{STATE::OAM_SCAN};

    int m_current_dot{0};
    int m_current_line{-1};
};

#endif