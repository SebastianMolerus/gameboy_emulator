#ifndef PPU_IMPL_HPP
#define PPU_IMPL_HPP

#include <ppu.hpp>
#include "pixel_fetcher.hpp"
#include <vector>

struct ppu::ppu_impl
{
    ppu_impl(rw_device &rw_device, drawing_device &drawing_device);
    rw_device &m_rw_device;
    drawing_device &m_drawing_device;
    pixel_fetcher m_pixel_fetcher;

    // temporary value of 0xFF40 ( lcd control )
    uint8_t m_lcd_ctrl{};

    // temporary value of visible sprites in each drawing line
    std::vector<sprite> visible_sprites{};

    bool draw_pixel_line();

    void dot();

    int dma_counter{};
    uint16_t dma_source{};
    void dma(uint8_t src_addr);
    void execute_dma();

    STATE m_current_state{STATE::OAM_SCAN};

    STATE current_state() const;

    void OAM_SCAN();
    void DRAWING_PIXELS();
    void HORIZONTAL_BLANK();
    void VERTICAL_BLANK();

    int m_current_dot{};
    int m_current_line{};
};

#endif