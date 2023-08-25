#include "cartridgeloader.hpp"

bool CartridgeLoader::load(std::filesystem::path &rPathToRom, std::span<char> buffer) 
{
    if (rPathToRom.empty()) return false;
    if (buffer.empty()) return false;

    if (rPathToRom.extension() != "gb")
        return false;

    romFile.open(rPathToRom, std::ifstream::in | std::ifstream::binary);

    if (!romFile.is_open()) return false;

    if (getFileSize() > buffer.size()) return false;

    romFile.read(buffer.data(), getFileSize());

    romFile.close();
    return true;
}

int CartridgeLoader::getFileSize() { 
   romFile.seekg (0, romFile.end);
    int length = romFile.tellg();
    romFile.seekg (0, romFile.beg);
    return length; 
}
