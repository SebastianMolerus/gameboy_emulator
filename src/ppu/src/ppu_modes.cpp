#include "ppu_impl.hpp"
#include <iostream>

namespace
{

sprite load_sprite(rw_device &rw, uint16_t addr)
{
    sprite result;
    result.m_y_pos = rw.read(addr++, device::PPU);
    result.m_x_pos = rw.read(addr++, device::PPU);
    result.m_tile_index = rw.read(addr++, device::PPU);
    result.m_flags = rw.read(addr, device::PPU);
    return result;
}

// Address where sprites reside, there are 40x of them
constexpr uint16_t OAM_ADDR{0xFE00};

bool check_line{};

} // namespace

void ppu::ppu_impl::OAM_SCAN()
{
    // when OBJ is enabled in FF40 - LCD Control
    // Do it once for each line
    if (checkbit(m_lcd_ctrl, 1) && !check_line)
    {
        update_stat(STATE::OAM_SCAN);

        check_line = true;
        visible_sprites.clear();
        uint8_t const sprite_high = checkbit(m_lcd_ctrl, 2) ? 16 : 8;

        for (int addr = OAM_ADDR; addr < OAM_ADDR + (40 * sizeof(sprite)); addr += sizeof(sprite))
        {
            sprite s{load_sprite(m_rw_device, addr)};
            if (m_current_line >= (s.m_y_pos - 16) && m_current_line <= (s.m_y_pos - 16 + sprite_high))
            {
                visible_sprites.push_back(s);
                if (visible_sprites.size() == 10)
                    break;
            }
        }
    }

    if (m_current_dot == 80)
    {
        check_line = false;
        m_pixel_fetcher.update_addresses();
        m_pixel_fetcher.set_background_mode();
        m_current_state = STATE::DRAWING_PIXELS;
        update_stat(STATE::DRAWING_PIXELS);
    }
}

void ppu::ppu_impl::DRAWING_PIXELS()
{
    if (draw_pixel_line())
    {
        m_current_state = STATE::HORIZONTAL_BLANK;
        update_stat(STATE::HORIZONTAL_BLANK);
    }
}

void ppu::ppu_impl::HORIZONTAL_BLANK()
{
    if (m_current_dot == 456)
    {
        m_current_dot = 0;
        ++m_current_line;
        if (m_current_line == 144)
        {
            m_current_state = STATE::VERTICAL_BLANK;
            update_stat(STATE::VERTICAL_BLANK);
            auto interrupt_flag = m_rw_device.read(INTERRUPT_FLAG, device::PPU); // load old if
            setbit(interrupt_flag, 0);                                           // activate Vblank
            m_rw_device.write(INTERRUPT_FLAG, interrupt_flag, device::PPU);      // save new if
        }
        else
        {
            m_current_state = STATE::OAM_SCAN;
        }
    }
}

void ppu::ppu_impl::VERTICAL_BLANK()
{
    if (m_current_dot == 456)
    {
        m_current_dot = -1;
        ++m_current_line;
        if (m_current_line == 154)
        {
            m_current_line = 0;
            m_current_state = STATE::OAM_SCAN;
            m_drawing_device.after_frame();
        }
    }
}
