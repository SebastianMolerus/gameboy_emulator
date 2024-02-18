#ifndef PIXEL_FETCHER_HPP
#define PIXEL_FETCHER_HPP

#include <common.hpp>
#include <queue>
#include <array>

class pixel_fetcher
{
  public:
    enum class LINE_DRAWING_STATUS
    {
        IN_PROGRESS,
        DONE
    };

    explicit pixel_fetcher(rw_device &rw_device, drawing_device &dd);
    LINE_DRAWING_STATUS dot(uint8_t screen_line);

  private:
    std::queue<color> pixel_fifo;
    std::array<color, 8> pixels;

    uint8_t x{};

    uint8_t pushed_px{};

    int dot_counter{};

    drawing_device &dd;
    rw_device &rw;
};

#endif