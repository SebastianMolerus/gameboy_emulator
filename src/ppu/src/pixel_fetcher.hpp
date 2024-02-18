#ifndef PIXEL_FETCHER_HPP
#define PIXEL_FETCHER_HPP

#include <common.hpp>
#include <queue>

class pixel_fetcher
{
  public:
    explicit pixel_fetcher(rw_device &rw_device, drawing_device &dd);
    void dot();

  private:
    std::queue<color> q;

    uint8_t x{};
    uint8_t y{};

    int cc{};

    uint16_t tile_line_addr{};
    uint8_t tile_lo{};
    uint8_t tile_hi{};

    uint8_t get_x();
    uint8_t get_y();

    drawing_device &dd;
    rw_device &rw;
};

#endif