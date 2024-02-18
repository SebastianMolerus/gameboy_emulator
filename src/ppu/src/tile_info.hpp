#ifndef TILE_INFO_HPP
#define TILE_INFO_HPP

#include <common.hpp>

class tile_info
{
  public:
    tile_info(rw_device &r);
    uint16_t get_tile_line_addr(uint8_t x, uint8_t y);

  private:
    uint16_t get_tile_map_addr();
    uint16_t get_tile_data_addr();

    size_t get_tile_offset(uint8_t x, uint8_t y);

    rw_device &rw;
};

#endif