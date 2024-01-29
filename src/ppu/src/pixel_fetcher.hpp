#ifndef PIXEL_FETCHER_HPP
#define PIXEL_FETCHER_HPP

#include <common.hpp>
#include <queue>
#include <array>

size_t get_tile_offset(int pix_x, int pix_y);
size_t get_tile_data_offset(int pix_x, int pix_y);
uint16_t get_tile_line(rw_device &r, int pix_x, int pix_y);
std::array<uint8_t, 8> convert_line_to_ids(uint16_t line);

class pixel_fetcher
{
  public:
    pixel_fetcher(rw_device &rw_device, drawing_device &drawing_device);
    bool dot();

  private:
    rw_device &m_rw_device;
    drawing_device &m_drawing_device;
    std::queue<int> m_pixel_fifo;
    int m_curr_line{};
};

#endif