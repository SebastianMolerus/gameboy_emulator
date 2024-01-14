#ifndef PPU_HPP
#define PPU_HPP

#include <memory>

struct rw_device;

class ppu
{
  public:
    ppu(rw_device &rw_device);
    ~ppu();
    void tick();
    struct ppu_impl;

  private:
    std::unique_ptr<ppu_impl> m_pimpl;
};

#endif