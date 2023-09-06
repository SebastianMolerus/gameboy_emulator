#ifndef COMMON_HPP
#define COMMON_HPP

#include <array>

/*  "ADC",        "ADD",        "AND",        "CALL",       "CCF",        "CP",         "CPL",        "DAA",
    "DEC",        "DI",         "EI",         "HALT",       "ILLEGAL_D3", "ILLEGAL_DB", "ILLEGAL_DD", "ILLEGAL_E3",
    "ILLEGAL_E4", "ILLEGAL_EB", "ILLEGAL_EC", "ILLEGAL_ED", "ILLEGAL_F4", "ILLEGAL_FC", "ILLEGAL_FD", "INC",
    "JP",         "JR",         "LD",         "LDH",        "NOP",        "OR",         "POP",        "PREFIX",
    "PUSH",       "RET",        "RETI",       "RLA",        "RLCA",       "RRA",        "RRCA",       "RST",
    "SBC",        "SCF",        "STOP",       "SUB",        "XOR",        "BIT",        "CALL",       "RLC",
    "RRC",        "RL",         "SWAP",       "RES",        "SET",        "SRA",        "SRL",        "RR",
    "SLA" */
extern std::array<const char *, 57> MNEMONICS_STR;

/*"$00", "$08", "$10", "$18", "$20", "$28", "$30", "$38", "A",   "AF", "B", "BC", "C", "D", "DE", "E", "H", "HL",
    "L",   "NC",  "NZ",  "SP",  "Z",   "a16", "a8",  "e8",  "n16", "n8", "0", "1",  "2", "3", "4",  "5", "6", "7" */
extern std::array<const char *, 36> OPERANDS_STR;

/* "Z", "N", "H", "C", "0", "1", "-" */
extern std::array<const char *, 7> FLAGS_STR;

#endif