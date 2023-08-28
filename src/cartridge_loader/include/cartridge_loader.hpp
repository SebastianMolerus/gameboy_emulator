#ifndef CARTRIDGELOADER_HPP
#define CARTRIDGELOADER_HPP
#include <filesystem>
#include <span>

namespace CartridgeLoader 
{
    bool load(std::filesystem::path &path_to_rom, std::span<uint8_t> buffer);
}

#endif /* CARTRIDGELOADER_HPP */ 