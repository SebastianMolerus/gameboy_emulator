#ifndef PPU_HPP
#define PPU_HPP

#include <memory>

struct rw_device;
struct drawing_device;

enum class STATE
{
    OAM_SCAN,
    DRAWING_PIXELS,
    HORIZONTAL_BLANK,
    VERTICAL_BLANK
};

class ppu
{
  public:
    ppu(rw_device &rw_device, drawing_device &drawing_device);
    ~ppu();

    void dot();

    // src_addr can be 0x00 to 0xDF
    void dma(uint8_t src_addr);
    STATE current_state() const;
    struct ppu_impl;

  private:
    std::unique_ptr<ppu_impl> m_pimpl;
};

#endif