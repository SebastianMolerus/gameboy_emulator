#include <assembler.hpp>
#include <gtest/gtest.h>
#include <utils.h>

// 0x20
TEST(test_jmp, JR_NZ_e8)
{
    std::string assembly{R"(
        JR NZ, 0x2  ;  1. jump to [3]
        JR NZ, 0x2  ;  2. jump to [4]
        JR NZ, 0x84 ;  3. jump to [2] because 0x84 == (-4)
        LD A, 0x5   ;  4.
        LD A, 0x0
        LD B, 0x0
        ADD A, B    ;  Z flag is set after ADD A, B
        JR NZ, 0x3  ;  no jump
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(8, assembly);

    ASSERT_EQ(expected_data[0].PC(), 4);

    ASSERT_EQ(expected_data[1].PC(), 2);

    ASSERT_EQ(expected_data[2].PC(), 6);

    ASSERT_EQ(expected_data[3].A(), 0x5);

    ASSERT_TRUE(expected_data[6].is_flag_set(flag::Z));

    ASSERT_EQ(expected_data[7].PC(), 0xF);

    ASSERT_EQ(wait_cycles[0], 12);
    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[7], 8);
}

// 0x30
TEST(test_jmp, JR_NC_e8)
{
    std::string assembly{R"(
        JR NC, 0x2
        JR NC, 0x2
        JR NC, 0x84
        LD A, 0x5
        LD A, 0x80
        LD B, 0x80
        ADD A, B
        JR NC, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(8, assembly);

    ASSERT_EQ(expected_data[0].PC(), 4);
    ASSERT_EQ(expected_data[1].PC(), 2);
    ASSERT_EQ(expected_data[2].PC(), 6);
    ASSERT_EQ(expected_data[3].A(), 0x5);

    ASSERT_TRUE(expected_data[6].is_flag_set(flag::C));

    // No jump in last instruction because
    // C flag is set
    ASSERT_EQ(expected_data[7].PC(), 0xF);

    ASSERT_EQ(wait_cycles[0], 12);
    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[7], 8);
}

// 0x28
TEST(test_jmp, JR_Z_e8)
{
    std::string assembly{R"(
        ADD A, B
        JR Z, 0x2
        JR Z, 0x2
        JR Z, 0x84
        LD A, 0x5
        LD A, 0x0
        LD B, 0x5
        ADD A, B
        JR Z, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(9, assembly);

    ASSERT_EQ(expected_data[0].A(), 0x0);

    ASSERT_EQ(expected_data[1].PC(), 5);
    ASSERT_EQ(expected_data[2].PC(), 3);
    ASSERT_EQ(expected_data[3].PC(), 7);
    ASSERT_EQ(expected_data[4].A(), 0x5);

    ASSERT_FALSE(expected_data[7].is_flag_set(flag::Z));

    // No jump in last instruction because
    // Z flag is NOT set
    ASSERT_EQ(expected_data[8].PC(), 0x10);

    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[2], 12);
    ASSERT_EQ(wait_cycles[3], 12);
    ASSERT_EQ(wait_cycles[8], 8);
}

// 0x38
TEST(test_jmp, JR_C_e8)
{
    std::string assembly{R"(
        LD A, 0xFF
        LD B, 0x80
        ADD A, B
        JR C, 0x2
        JR C, 0x2
        JR C, 0x84
        LD A, 0x5
        LD A, 0x5
        LD B, 0x5
        ADD A, B
        JR C, 0x3
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(11, assembly);

    ASSERT_TRUE(expected_data[2].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[3].PC(), 9);
    ASSERT_EQ(expected_data[4].PC(), 7);
    ASSERT_EQ(expected_data[5].PC(), 11);
    ASSERT_EQ(expected_data[6].A(), 0x5);
    ASSERT_FALSE(expected_data[9].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[10].PC(), 0x14);
}

// 0x18
TEST(test_jmp, JR_e8)
{
    std::string assembly{R"(
        LD A, 0x99
        JR 0x2
        LD A, 0x3
        JR 0x84
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(4, assembly);

    ASSERT_EQ(expected_data[1].PC(), 0x6);

    ASSERT_EQ(expected_data[2].PC(), 0x4);
    ASSERT_EQ(expected_data[2].A(), 0x99);

    ASSERT_EQ(expected_data[3].PC(), 0x6);
    ASSERT_EQ(expected_data[3].A(), 0x3);
}

// 0xC2
TEST(test_jmp, JP_NZ_a16)
{
    std::string assembly{R"(
        JP NZ, 0x5
        LD B, 0x5
        JP NZ, 0xA
        LD D, 0xFF
        ADD A, B
        JP NZ, 0xFFFF
    )"};

    auto [expected_data, wait_cycles] = get_cpu_output(4, assembly);

    ASSERT_EQ(expected_data[1].B(), 0x0);
    ASSERT_EQ(expected_data[1].D(), 0x0);

    ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));

    // No jump because Z flag is set
    ASSERT_EQ(expected_data[3].PC(), 0xE);
    ASSERT_EQ(expected_data[3].D(), 0x0);

    ASSERT_EQ(wait_cycles[0], 16);
    ASSERT_EQ(wait_cycles[1], 16);
    ASSERT_EQ(wait_cycles[3], 12);
}

// 0xD2
TEST(test_jmp, JP_NC_a16)
{
    std::string assembly{R"(
        JP NC, 0x9b78   ; 1. jump to [2]
        LD B, 0x32      ; this instruction is avoided
        LD D, 0x16      ; 4.
        LD A, 0x80      ; 5.
        LD B, 0x80      ; 6.
        ADD A, B        ; 7. C flag should be set
        JP NC, 0xABCD   ; 8. there should be no jump
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x9b78, R"(
        LD A, 0xB5      ; 2.
        JP NC, 0x5      ; 3. jump to [4]
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].PC(), 0x9b78);
    ASSERT_EQ(expected_data[1].A(), 0xB5);
    ASSERT_EQ(expected_data[2].PC(), 0x5);
    ASSERT_EQ(expected_data[3].D(), 0x16);
    ASSERT_TRUE(expected_data[6].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[7].PC(), 0xF);

    ASSERT_EQ(wait_cycles[0], 16);
    ASSERT_EQ(wait_cycles[2], 16);
    ASSERT_EQ(wait_cycles[7], 12);
}

// 0xCA
TEST(test_jmp, JP_Z_a16)
{
    std::string assembly{R"(
        ADD A, B       ;      1. Z is set
        JP Z, 0x0bf1   ;      2. jump to 3
        ADD A, B       ;      5. Z is reset ( B is 0xCA )
        JP Z, 0x7646   ;      6. No Jump, PC == 5
        LD D, 0x4f     ;      7.
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x0bf1, R"(
        LD B, 0xca      ; 3.
        JP Z, 0x4       ; 4. jump to [5]
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_TRUE(expected_data[0].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[1].PC(), 0x0bf1);
    ASSERT_EQ(expected_data[2].B(), 0xca);
    ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[3].PC(), 0x4);
    ASSERT_FALSE(expected_data[4].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[4].PC(), 0x5);
    ASSERT_EQ(expected_data[6].D(), 0x4f);

    ASSERT_EQ(wait_cycles[1], 16);
    ASSERT_EQ(wait_cycles[3], 16);
    ASSERT_EQ(wait_cycles[5], 12);
}

// 0xDA
TEST(test_jmp, JP_C_a16)
{
    std::string assembly{R"(
        LD A, 0x80      ; 0. 
        JP C, 0x5def    ; 1. no jump
        LD B, 0x80      ; 2.
        ADD A, B        ; 3. C is Set
        JP C, 0x38a     ; 4. 
        LD E, 0xf3      ; 7.
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x38a, R"(
        LD H, 0x35      ; 5
        JP C, 0xb       ; 6
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[1].PC(), 0x5);
    ASSERT_TRUE(expected_data[3].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[4].PC(), 0x38a);
    ASSERT_EQ(expected_data[5].H(), 0x35);
    ASSERT_EQ(expected_data[6].PC(), 0xb);
    ASSERT_EQ(expected_data[7].E(), 0xf3);

    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[4], 16);
    ASSERT_EQ(wait_cycles[6], 16);
}

// 0xC3
TEST(test_jmp, JP_a16)
{
    std::string assembly{R"(
        JP 0x72a4   ; 0 
        JP 0x72a7   ; 2
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x72a4, R"(
        JP 0x3      ; 1
        JP 0x6      ; 3
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].PC(), 0x72a4);
    ASSERT_EQ(expected_data[1].PC(), 0x3);
    ASSERT_EQ(expected_data[2].PC(), 0x72a7);
    ASSERT_EQ(expected_data[3].PC(), 0x6);

    ASSERT_EQ(wait_cycles[0], 16);
    ASSERT_EQ(wait_cycles[1], 16);
    ASSERT_EQ(wait_cycles[2], 16);
    ASSERT_EQ(wait_cycles[3], 16);
}

// 0xCD
TEST(test_jmp, CALL_a16)
{
    std::string assembly{R"(
        LD SP, 0x8514 ; 0. 
        LD A, 0x5     ; 1.
        CALL 0x9443   ; 2. Address of next instruction is 0x8, push it into stack, jump to [3]
        LD A, 0x9     ;    This instruction is avoided
        LD B, 0x15    ; 5.
        JP 0x9448     ; 6. jump to [7]
        NOP           ; 8.
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x9443, R"(
       LD A, 0x10     ; 3.
       JP 0xA         ; 4. jump to [5]  
       CALL 0xF       ; 7. jump to [8]
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].SP(), 0x8514);
    ASSERT_EQ(expected_data[2].SP(), 0x8512);
    ASSERT_EQ(expected_data[5].A(), 0x10);
    ASSERT_EQ(expected_data[5].B(), 0x15);

    ASSERT_EQ(expected_data[7].SP(), 0x8510);
    ASSERT_EQ(expected_data[7].B(), 0x15);

    ASSERT_EQ(mock.m_ram[0x8512], 0x8);
    ASSERT_EQ(mock.m_ram[0x8513], 0x0);
    ASSERT_EQ(mock.m_ram[0x8510], 0x4B);
    ASSERT_EQ(mock.m_ram[0x8511], 0x94);

    ASSERT_EQ(wait_cycles[2], 24);
    ASSERT_EQ(wait_cycles[7], 24);
}

// 0xC4
// CALL NZ, a16
TEST(test_jmp, CALL_NZ_a16)
{
    std::string assembly{R"(
        LD SP, 0xFF00    ; 0. [3B]
        CALL NZ, 0x6dad  ; 1. [3B] ; push 0x6
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x6dad, R"(
        ADD A, B        ; 2. [1B] Z flag set
        CALL NZ, 0xB    ; 3. [3B] No jump
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].SP(), 0xFF00);
    ASSERT_FALSE(expected_data[0].is_flag_set(flag::Z));

    ASSERT_EQ(expected_data[1].SP(), 0xFEFE);
    ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));

    ASSERT_EQ(expected_data[3].SP(), 0xFEFE);
    ASSERT_EQ(expected_data[3].PC(), 0x6db1);

    ASSERT_EQ(mock.m_ram[0xFEFE], 0x6);
    ASSERT_EQ(mock.m_ram[0xFEFF], 0x0);

    ASSERT_EQ(wait_cycles[1], 24);
    ASSERT_EQ(wait_cycles[3], 12);
}

// 0xCC
// CALL Z, a16
TEST(test_jmp, CALL_Z_a16)
{
    std::string assembly{R"(
        LD SP, 0xFFFF      ; 0. [3B]
        CALL Z, 0xa43f     ; 1. [3B] no jump
        ADD A, B           ; 2. [1B] Z is set
        CALL Z, 0xa43f     ; 3. [3B] jump
    )"};

    rw_mock mock(assembly);

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].SP(), 0xFFFF);
    ASSERT_EQ(expected_data[1].PC(), 0x6);
    ASSERT_EQ(expected_data[1].SP(), 0xFFFF);
    ASSERT_TRUE(expected_data[2].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[3].PC(), 0xa43f);
    ASSERT_EQ(expected_data[3].SP(), 0xFFFD);

    ASSERT_EQ(mock.m_ram[0xFFFD], 0xA);
    ASSERT_EQ(mock.m_ram[0xFFFE], 0x0);

    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[3], 24);
}

// 0xDC
// CALL C, a16
TEST(test_jmp, CALL_C_a16)
{
    std::string assembly{R"(
        LD SP, 0xFFFF           ; 0. [3B]
        CALL C, 0xc271          ; 1. [3B] no jump
        LD A, 0x80              ; 2. [2B]
        LD B, 0x80              ; 3. [2B]
        ADD A, B                ; 4. [1B] C is set
        CALL C, 0xc271          ; 5. [3B] jump
    )"};

    rw_mock mock(assembly);

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].SP(), 0xFFFF);
    ASSERT_EQ(expected_data[1].PC(), 0x6);
    ASSERT_EQ(expected_data[1].SP(), 0xFFFF);
    ASSERT_TRUE(expected_data[4].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[5].PC(), 0xc271);
    ASSERT_EQ(expected_data[5].SP(), 0xFFFD);

    ASSERT_EQ(mock.m_ram[0xFFFD], 0xE);
    ASSERT_EQ(mock.m_ram[0xFFFE], 0x0);

    ASSERT_EQ(wait_cycles[1], 12);
    ASSERT_EQ(wait_cycles[5], 24);
}

// 0xD4
// CALL NC, a16
TEST(test_jmp, CALL_NC_a16)
{
    std::string assembly{R"(
        LD SP, 0xb8e1           ; 0. [3B]
        LD A, 0x80              ; 1. [2B]
        LD B, 0x80              ; 2. [2B]
        ADD A, B                ; 3. [1B], C is set
        CALL NC, 0x1669         ; 4. [3B], no jump
        LD A, 0x0               ; 5. [2B]
        ADD A, B                ; 6. [1B], C is reset
        CALL NC, 0x1669         ; 7. [3B], jump
    )"};

    rw_mock mock(assembly);

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].SP(), 0xb8e1);
    ASSERT_TRUE(expected_data[3].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[4].PC(), 0xB);
    ASSERT_EQ(expected_data[4].SP(), 0xb8e1);
    ASSERT_FALSE(expected_data[6].is_flag_set(flag::C));

    ASSERT_EQ(expected_data[7].PC(), 0x1669);
    ASSERT_EQ(expected_data[7].SP(), 0xb8dF);

    ASSERT_EQ(mock.m_ram[0xb8e0], 0x0);
    ASSERT_EQ(mock.m_ram[0xb8dF], 0x11);
}

// 0xE9
// JP HL
TEST(test_jmp, JP_HL)
{
    std::string assembly{R"(
        LD HL, 0xe386       ; 0. [3B]
        JP HL               ; 1. [1B]
    )"};

    rw_mock mock(assembly);

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[0].PC(), 0x3);
    ASSERT_EQ(expected_data[1].PC(), 0xe386);
    ASSERT_EQ(wait_cycles[1], 0x4);
}

// 0xC9
// RET
TEST(test_jmp, RET)
{
    std::string assembly{R"(
        LD A, 0x4           ; [2B]  0.
        LD B, 0x5           ; [2B]  1.
        LD SP, 0xFF61       ; [3B]  2.
        CALL 0x6a32         ; [3B]  3.
        LD A, 0x98          ; [2B]  5. 
        LD B, 0x61          ; [2B]  6. 
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x6a32, R"(
        RET ;     4.
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[2].SP(), 0xFF61);
    ASSERT_EQ(expected_data[3].PC(), 0x6a32);
    ASSERT_EQ(expected_data[3].SP(), 0xFF5F);
    ASSERT_EQ(expected_data[4].PC(), 0xA);
    ASSERT_EQ(expected_data[5].A(), 0x98);
    ASSERT_EQ(expected_data[6].B(), 0x61);

    ASSERT_EQ(wait_cycles[4], 16);
};

// 0xC0
// RET NZ
TEST(test_jmp, RET_NZ)
{
    std::string assembly{R"(
        LD SP, 0xef3b       ; [3B]  0.
        CALL 0x10ad         ; [3B]  1. 
        ADD A, B            ; [1B]  3. Z Flag is set
        CALL 0x10ae         ; [3B]  4. 
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x10ad, R"(
        RET NZ          ;  2. [1B] 
        LD A, 0x5       ;  5. [2B]
        RET NZ          ;  6. [1B] Z is set, no jump
        LD A, 0x6       ;  7.
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[1].PC(), 0x10ad);
    ASSERT_FALSE(expected_data[1].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[2].PC(), 0x6);
    ASSERT_TRUE(expected_data[3].is_flag_set(flag::Z));
    ASSERT_EQ(expected_data[4].PC(), 0x10ae);
    ASSERT_EQ(expected_data[6].PC(), 0x10b1);

    ASSERT_EQ(wait_cycles[2], 20);
    ASSERT_EQ(wait_cycles[6], 8);
};

// 0xD0
// RET NC
TEST(test_jmp, RET_NC)
{
    std::string assembly{R"(
        LD SP, 0xbf15       ; [3B]  
        CALL 0x76ce         ; [3B]  1.
        LD A, 0x80          ; [2B]  3.
        LD B, 0x80          ; [2B]  
        ADD A, B            ; [1B]  5.  C is ON
        CALL 0x76cf         ; [3B]  6.
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0x76ce, R"(
        RET NC          ;    2. ( go to 3 )
        LD A, 0x5       ;    7.
        RET NC          ;    8. ( no jump here )
        LD A, 0x6       ;    9.
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[1].PC(), 0x76ce);
    ASSERT_FALSE(expected_data[1].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[2].PC(), 0x6);
    ASSERT_TRUE(expected_data[7].is_flag_set(flag::C));
    ASSERT_EQ(expected_data[8].PC(), 0x76ce + 4);
    ASSERT_EQ(expected_data[9].A(), 0x6);

    ASSERT_EQ(wait_cycles[2], 20);
    ASSERT_EQ(wait_cycles[8], 8);
};

// 0xC8
TEST(test_jmp, RET_Z)
{
    std::string assembly{R"(
        LD SP, 0xFFFF       ; 0.
        CALL 0xae11         ; 1.
        LD A, 0x57          ; 5.
    )"};

    rw_mock mock(assembly);

    mock.add_instruction_at(0xae11, R"(
        RET Z        ; 2. No jump
        ADD A, B     ; 3. Z flag is set after this
        RET Z        ; 4. Jump
    )");

    auto [expected_data, wait_cycles] = mock.get_cpu_output();

    ASSERT_EQ(expected_data[1].PC(), 0xae11);
    ASSERT_EQ(expected_data[2].PC(), 0xae11 + 1);
    ASSERT_EQ(expected_data[4].PC(), 0x6);

    ASSERT_EQ(wait_cycles[2], 8);
    ASSERT_EQ(wait_cycles[4], 20);
};
