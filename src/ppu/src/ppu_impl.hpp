#ifndef PPU_IMPL_HPP
#define PPU_IMPL_HPP

#include <ppu.hpp>

struct ppu::ppu_impl
{
    ppu_impl(rw_device &rw_device);
    rw_device &m_rw_device;
    void tick();
};

#endif