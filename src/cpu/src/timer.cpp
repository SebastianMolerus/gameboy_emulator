#include "cpu_impl.hpp"

namespace
{
int DIV_CC{256};
}

// It is called on each cpu tick
void cpu::cpu_impl::timer()
{
    --DIV_CC;
    if (!DIV_CC)
    {
        uint8_t div_timer_value = m_rw_device.read(0xFF04);
        ++div_timer_value;
        m_rw_device.write(0xFF04, div_timer_value);
        DIV_CC = 256;
    }
}