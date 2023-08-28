#include "cartridge_loader.hpp"
#include <fstream>


namespace CartridgeLoader
{
namespace 
{

size_t get_file_size(std::ifstream &rom_file)
{ 
    rom_file.seekg (0, rom_file.end);
    size_t length = rom_file.tellg();
    rom_file.seekg (0, rom_file.beg);
    return length; 
}

}

bool load(std::filesystem::path &path_to_rom, std::span<uint8_t> buffer) 
{
    if (path_to_rom.empty()) return false;
    if (buffer.empty()) return false;
    if (path_to_rom.extension() != ".gb") return false;

    std::ifstream rom_file;

    rom_file.open(path_to_rom, std::ifstream::in | std::ifstream::binary);

    if (!rom_file.is_open()) return false;

    if (get_file_size(rom_file) > buffer.size()) return false;

    rom_file.read(reinterpret_cast<char*>(buffer.data()), get_file_size(rom_file));

    rom_file.close();
    return true;
}

}