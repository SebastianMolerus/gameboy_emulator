#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <common.hpp>
#include <vector>

class memory : public rw_device
{
  public:
    memory();
    uint8_t read(uint16_t addr, device d = device::CPU) override;
    void write(uint16_t addr, uint8_t data, device d = device::CPU) override;

  private:
    struct memory_area
    {
        std::vector<uint8_t> m_mem;
        uint16_t m_beg;
        uint16_t m_end;
    };

    static std::vector<memory_area> DMG_MEMORY;
    template <uint16_t beg, uint16_t end = beg> struct memory_block
    {
        memory_block()
        {
            memory_area new_area{std::vector<uint8_t>(end - beg + 1, {}), beg, end};
            DMG_MEMORY.push_back(std::move(new_area));
        }
    };

    void swap_rom();

    memory_block<0, 0xFF> m_BOOT_ROM; // swappable with m_ROM
    memory_block<0, 0x7FFF> m_ROM;    // swappable with m_BOOT_ROM
    memory_block<0x8000, 0x9fff> m_VRAM;
    memory_block<0xA000, 0xBFFF> m_EXTERNAL_RAM;
    memory_block<0xC000, 0xCFFF> m_RAM;
    memory_block<0xD000, 0xDFFF> m_RAM_2;
    memory_block<0xE000, 0xFDFF> m_ECHO_RAM;
    memory_block<0xFE00, 0xFE9F> m_OAM;
    memory_block<0xFF00, 0xFF7F> m_IO_REG;
    memory_block<0xFF80, 0xFFFE> m_HIGH_RAM;
    memory_block<0xFEA0, 0xFEFF> m_NOT_USABLE;
    memory_block<0xFFFF> m_IE;
};

#endif