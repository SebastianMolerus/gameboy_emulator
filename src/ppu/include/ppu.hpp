#ifndef PPU_HPP
#define PPU_HPP

#include <memory>

struct rw_device;
struct drawing_device;

class ppu
{
  public:
    ppu(rw_device &rw_device, drawing_device &drawing_device);
    ~ppu();

    void dot();

    // src_addr can be 0x00 to 0xDF
    void dma(uint8_t src_addr);
    struct ppu_impl;

  private:
    std::unique_ptr<ppu_impl> m_pimpl;
};

#endif