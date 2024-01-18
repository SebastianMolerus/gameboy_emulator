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

    bool dot();
    struct ppu_impl;

  private:
    std::unique_ptr<ppu_impl> m_pimpl;
};

#endif