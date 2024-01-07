#include <cpu.hpp>
#include <decoder.hpp>
#include <iomanip>
#include <iostream>


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