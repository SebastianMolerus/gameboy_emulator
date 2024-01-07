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

// range == <begin, end>
template <uint16_t begin, uint16_t end = begin> struct memory_block
{
    static constexpr uint16_t m_begin{begin};
    static constexpr uint16_t m_end{end};
    std::vector<uint8_t> m_block;
    memory_block() : m_block(m_end - m_begin + 1, {})
    {
    }

    uint8_t &operator[](int index)
    {
        assert(index >= m_begin && index <= m_end);
        return m_block[index - m_begin];
    }

    bool in_range(uint16_t addr)
    {
        return addr >= m_begin && addr <= m_end;
    }
};

struct dmg : public rw_device
{
    memory_block<0, 0xFF> m_BOOT_ROM;
    memory_block<0x8000, 0x9fff> m_VRAM;
    memory_block<0xFF26> m_AUDIO_MASTER_CTRL;

    cpu m_cpu;
    dmg() : m_cpu{*this, cpu_cb}
    {
        auto const rom{load_boot_rom()};
        for (int i = 0; i < m_BOOT_ROM.m_block.size(); ++i)
            m_BOOT_ROM.m_block[i] = rom[i];
    }

    uint8_t read(uint16_t addr) override
    {
        uint8_t ret_value{0xFF};

        if (m_BOOT_ROM.in_range(addr))
            ret_value = m_BOOT_ROM[addr];
        else if (m_VRAM.in_range(addr))
            ret_value = m_VRAM[addr];

        return ret_value;
    }

    void write(uint16_t addr, uint8_t data) override
    {
        if (m_VRAM.in_range(addr))
            m_VRAM[addr] = data;
        else if (m_AUDIO_MASTER_CTRL.in_range(addr))
            m_AUDIO_MASTER_CTRL[addr] = data;
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