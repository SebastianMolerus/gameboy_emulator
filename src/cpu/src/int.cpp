#include "cpu_impl.hpp"
#include "common.hpp"

namespace
{
cpu::cpu_impl *g_cpu{};
uint8_t IE{};
uint8_t IF{};

constexpr uint16_t IF_ADDR{0xFF0F};

constexpr uint8_t VBLANK_BIT{0};
constexpr uint8_t VBLANK_JUMP_ADDR{0x40};

constexpr uint8_t STAT_BIT{1};
constexpr uint8_t STAT_JUMP_ADDR{0x48};

constexpr uint8_t TIMER_BIT{2};
constexpr uint8_t TIMER_JUMP_ADDR{0x50};

constexpr uint8_t SERIAL_BIT{3};
constexpr uint8_t SERIAL_JUMP_ADDR{0x58};

constexpr uint8_t JOYPAD_BIT{4};
constexpr uint8_t JOYPAD_JUMP_ADDR{0x60};

using im = cpu::cpu_impl::IME;

void int_handler(uint8_t bit_to_clear, uint16_t addr_to_jump)
{
    g_cpu->m_IME = im::DISABLED;
    g_cpu->m_rw_device.write(IF_ADDR, clearbit(IF, bit_to_clear));
    g_cpu->push_PC();
    g_cpu->m_reg.PC() = addr_to_jump;
}

} // namespace

void check_interrupt(cpu::cpu_impl &cpu)
{
    g_cpu = &cpu;

    if (cpu.m_IME != cpu::cpu_impl::IME::ENABLED)
        return;

    IE = cpu.m_rw_device.read(0xFFFF);
    IF = cpu.m_rw_device.read(0xFF0F);

    if (checkbit(IF & IE, VBLANK_BIT))
    {
        int_handler(VBLANK_BIT, VBLANK_JUMP_ADDR);
    }
    else if (checkbit(IF & IE, STAT_BIT))
    {
        int_handler(STAT_BIT, STAT_JUMP_ADDR);
    }
    else if (checkbit(IF & IE, TIMER_BIT))
    {
        int_handler(TIMER_BIT, TIMER_JUMP_ADDR);
    }
    else if (checkbit(IF & IE, SERIAL_BIT))
    {
        int_handler(SERIAL_BIT, SERIAL_JUMP_ADDR);
    }
    else if (checkbit(IF & IE, JOYPAD_BIT))
    {
        int_handler(JOYPAD_BIT, JOYPAD_JUMP_ADDR);
    }
}
