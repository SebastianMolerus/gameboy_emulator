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

int main()
{
    title();
    return 0;
}