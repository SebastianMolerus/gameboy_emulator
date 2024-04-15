#ifndef PIXEL_FETCHER_HPP
#define PIXEL_FETCHER_HPP

#include <common.hpp>

class pixel_fetcher
{
  public:
    explicit pixel_fetcher(rw_device &rw_device);
    uint16_t fetch_tile_line(screen_coordinates sc);
    void update_addresses();

  private:
    rw_device &m_rw;
};

#endif