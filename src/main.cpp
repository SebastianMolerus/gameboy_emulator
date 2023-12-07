#include "cartridge_loader.hpp"
#include "cpu.hpp"
#include <cstdint>
#include <filesystem>
#include <iostream>

using namespace std;

namespace
{
void title()
{
    cout << "  ____  __  __     ____ ____     _____                  \n";
    cout << " |  _ \\|  \\/  |   / ___| __ )   | ____|_ __ ___  _   _  \n";
    cout << " | |_) | |\\/| |  | |  _|  _ \\   |  _| | '_ ` _ \\| | | | \n";
    cout << " |  _ <| |  | |  | |_| | |_) |  | |___| | | | | | |_| | \n";
    cout << " |_| \\_\\_|  |_|___\\____|____/___|_____|_| |_| |_|\\__,_| \n";
    cout << "             |_____|       |_____|                      \n\n\n";
}
} // namespace

struct bus : public rw_device
{
    cpu m_cpu;
    std::array<uint8_t, 3> m_ram{0x01, 0xAA, 0xBB};

    bus() : m_cpu{*this}
    {
    }

    void start()
    {
        m_cpu.start();
    }

    virtual uint8_t read(uint16_t addr)
    {
        return m_ram[addr];
    }
    virtual void write(uint16_t addr, uint8_t data)
    {
        m_ram[addr] = data;
    }
};

int main()
{
    title();
    bus b;
    b.start();
    return 0;
}