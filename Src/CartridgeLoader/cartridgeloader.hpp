#ifndef CARTRIDGELOADER_HPP
#define CARTRIDGELOADER_HPP
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <span>

class CartridgeLoader
{
public:
    bool load(std::filesystem::path &rPathToRom, std::span<char> buffer);
private:
    int getFileSize();

    std::ifstream romFile;
};

#endif /* CARTRIDGELOADER_HPP */ 