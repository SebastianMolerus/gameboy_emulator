#include "src/Cpu/Cpu.hpp"
#include <iostream>

using namespace std;

static void title()
{
    cout << "  ____  __  __     ____ ____     _____                  \n";
    cout << " |  _ \\|  \\/  |   / ___| __ )   | ____|_ __ ___  _   _  \n";
    cout << " | |_) | |\\/| |  | |  _|  _ \\   |  _| | '_ ` _ \\| | | | \n";
    cout << " |  _ <| |  | |  | |_| | |_) |  | |___| | | | | | |_| | \n";
    cout << " |_| \\_\\_|  |_|___\\____|____/___|_____|_| |_| |_|\\__,_| \n";
    cout << "             |_____|       |_____|                      \n\n\n";
}

int main()
{
    title();
    Cpu cpu;
    cout << "Register A: " << (int)cpu.registers.A << endl;
    return 0;
}