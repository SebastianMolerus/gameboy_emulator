#include <array>
#include <cassert>
#include <cpu.hpp>
#include <decoder.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{

const std::filesystem::path boot_rom_file{BOOT_ROM_FILE};

std::array<uint8_t, 256> load_boot_rom()
{
    assert(std::filesystem::exists(boot_rom_file));
    std::ifstream ifs{boot_rom_file, std::ios_base::in | std::ios_base::binary};
    assert(ifs.is_open());
    std::array<uint8_t, 256> boot_rom{};
    ifs.read(reinterpret_cast<char *>(boot_rom.data()), boot_rom.size());
    return boot_rom;
}

void print_op(registers const &reg, opcode const &op)
{
    static int curr_line{};
    std::cerr << "0x" << std::setw(4) << std::setfill('0') << std::hex << curr_line << ": " << op.m_mnemonic << " ";

    for (auto i = 0; i < op.operands_size(); ++i)
    {
        auto const &oper = op.m_operands[i];

        auto const oper_name = oper.m_name;

        if (!std::strcmp(oper_name, "n8") || !std::strcmp(oper_name, "a8") || !std::strcmp(oper_name, "e8"))
            std::cerr << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(op.m_data[0]);
        else if (!std::strcmp(oper_name, "n16") || !std::strcmp(oper_name, "a16"))
            std::cerr << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(op.m_data[1])
                      << static_cast<int>(op.m_data[0]);
        else
            std::cerr << oper_name;

        if (!i && op.operands_size() > 1)
            std::cerr << ", ";
    }

    std::cerr << " \n";

    curr_line = reg.get_pc();
}

void cpu_cb(registers const &reg, opcode const &op)
{
    print_op(reg, op);
}

struct dmg : public rw_device
{
    static constexpr uint16_t VRAM_BEGIN{0x8000};
    static constexpr uint16_t VRAM_END{0x9FFF};
    std::vector<uint8_t> m_VRAM; // 8KiB

    static constexpr uint16_t BOOT_ROM_END{0xFF};
    std::array<uint8_t, 256> m_boot_rom;

    cpu m_cpu;
    dmg() : m_cpu{*this, cpu_cb}, m_boot_rom{load_boot_rom()}, m_VRAM(8192, {})
    {
    }

    uint8_t read(uint16_t addr) override
    {
        uint8_t ret_value{0xFF};

        if (addr <= BOOT_ROM_END)
            ret_value = m_boot_rom[addr];
        else if (addr >= 0x8000 && addr <= 0x9FFF)
        {
            addr -= 0x8000;
            ret_value = m_VRAM[addr];
        }

        return ret_value;
    }

    void write(uint16_t addr, uint8_t data) override
    {
        if (addr >= VRAM_BEGIN && addr <= VRAM_END)
        {
            addr -= VRAM_BEGIN;
            m_VRAM[addr] = data;
        }
    }

    void start()
    {
        while (1)
        {
            m_cpu.tick();
        }
    }
};

} // namespace

int main()
{
    dmg gameboy;
    gameboy.start();
    return 0;
}