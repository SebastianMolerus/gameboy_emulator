#include <common.hpp>
#include "ppu_impl.hpp"
#include <cassert>

ppu::ppu_impl::ppu_impl(rw_device &rw_device) : m_rw_device{rw_device}
{
}

void ppu::ppu_impl::tick()
{
}

// ******************************************
//                  CPU PART
// ******************************************
ppu::ppu(rw_device &rw_device) : m_pimpl{std::make_unique<ppu::ppu_impl>(rw_device)}
{
}

ppu::~ppu() = default;

void ppu::tick()
{
    assert(m_pimpl);
    m_pimpl->tick();
}
