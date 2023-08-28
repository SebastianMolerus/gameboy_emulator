#include <iostream>
#include <filesystem>
#include <cstdint>
#include "cpu.hpp"
#include "cartridge_loader.hpp"
#include "decoder.hpp"

using namespace std;

namespace {
void title()
{
    cout << "  ____  __  __     ____ ____     _____                  \n";
    cout << " |  _ \\|  \\/  |   / ___| __ )   | ____|_ __ ___  _   _  \n";
    cout << " | |_) | |\\/| |  | |  _|  _ \\   |  _| | '_ ` _ \\| | | | \n";
    cout << " |  _ <| |  | |  | |_| | |_) |  | |___| | | | | | |_| | \n";
    cout << " |_| \\_\\_|  |_|___\\____|____/___|_____|_| |_| |_|\\__,_| \n";
    cout << "             |_____|       |_____|                      \n\n\n";
}

void run_emulator() 
{
    Cpu cpu;
    Opcode opcode;
    std::filesystem::path path_to_rom = "/misc/oh.gb";
    std::filesystem::path path  = std::filesystem::current_path();
    path += path_to_rom;
    CartridgeLoader::load(path, cpu.memory);
    load_opcodes();

    uint8_t instruction = cpu.fetch_instruction(cpu.registers.PC);

    opcode = get_opcode(instruction);
    
    cout << opcode.mnemonic<< endl;
    ++cpu.registers.PC;
}
}

int main()
{
    title();
    run_emulator();
    return 0;
}