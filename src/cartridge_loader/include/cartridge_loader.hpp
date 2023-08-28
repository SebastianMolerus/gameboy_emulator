#ifndef CARTRIDGELOADER_HPP
#define CARTRIDGELOADER_HPP
#include <filesystem>
#include <span>

bool load(std::filesystem::path &rPathToRom, std::span<char> buffer);

#endif /* CARTRIDGELOADER_HPP */ 