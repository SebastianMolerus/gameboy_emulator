#include "cpu_impl.hpp"

namespace lol
{

constexpr int MC4{16};     // 262144 Hz
constexpr int MC16{64};    // 65536 Hz
constexpr int MC64{256};   // 16384 Hz
constexpr int MC256{1024}; // 4096 Hz

int DIV_CC{52};

int TIMA_CC{MC256};
int tima_freq{0};

int overflow_value{0};

} // namespace lol

using namespace lol;

// It is called on each cpu tick
void cpu::cpu_impl::timer()
{
    --DIV_CC;
    if (!DIV_CC)
    {
        uint8_t div_timer_value = m_rw_device.read(0xFF04);
        m_rw_device.write(0xFF04, ++div_timer_value, device::CPU, true);
        DIV_CC = MC64;
    }

    if (overflow_value >= 1)
    {
        ++overflow_value;

        if (overflow_value == 5)
        {
            uint8_t const timer_modulo = m_rw_device.read(0xFF06);
            m_rw_device.write(0xFF05, timer_modulo, device::CPU, true);

            TIMER_INT();
            overflow_value = 0;
            return;
        }
    }

    uint8_t const TAC = m_rw_device.read(0xFF07);
    if (checkbit(TAC, 2)) // enabled check
    {
        uint8_t const clock_selection = TAC & 0x03;

        uint8_t old_freq = tima_freq;

        switch (clock_selection)
        {
        case 0x00:
            tima_freq = MC256;
            break;
        case 0x01:
            tima_freq = MC4;
            break;
        case 0x10:
            tima_freq = MC16;
            break;
        case 0x11:
            tima_freq = MC64;
            break;
        }

        if (old_freq != tima_freq)
            TIMA_CC = tima_freq;

        --TIMA_CC;
        if (!TIMA_CC)
        {
            uint8_t const tima_counter_value = m_rw_device.read(0xFF05);

            m_rw_device.write(0xFF05, tima_counter_value + 1, device::CPU, true);
            TIMA_CC = tima_freq;

            if (tima_counter_value == 0xFF) // previous increment == overflow + INT
                overflow_value = 1;
        }
    }
}