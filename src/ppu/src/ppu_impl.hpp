#ifndef PPU_IMPL_HPP
#define PPU_IMPL_HPP

#include <ppu.hpp>
#include <lcd.hpp>

struct ppu::ppu_impl
{
    ppu_impl(rw_device &rw_device);
    rw_device &m_rw_device;
    std::unique_ptr<lcd> m_lcd;
    bool dot();

    int m_current_dot{0};
    int m_current_line{-1};

    void lcd_off_drawing();
    void lcd_white();
    void empty_frame();
};

#endif