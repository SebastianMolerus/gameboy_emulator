#ifndef PPU_HPP
#define PPU_HPP

#include <memory>

struct rw_device;

class ppu
{
  public:
    ppu(rw_device &rw_device);
    ~ppu();

    // One "dot" is equal to 4x ticks in CPU
    bool dot();
    struct ppu_impl;

  private:
    std::unique_ptr<ppu_impl> m_pimpl;
};

#endif