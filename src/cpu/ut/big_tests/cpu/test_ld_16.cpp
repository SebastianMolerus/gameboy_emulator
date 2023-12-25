#include "../big_tests_utils.h"

namespace
{
const std::filesystem::path LD_BC_n16_tests_path{test_data_dir / "cpu_tests" / "v1" / "01.json"};
const std::filesystem::path LD_DE_n16_tests_path{test_data_dir / "cpu_tests" / "v1" / "11.json"};
const std::filesystem::path LD_HL_n16_tests_path{test_data_dir / "cpu_tests" / "v1" / "21.json"};
const std::filesystem::path LD_SP_n16_tests_path{test_data_dir / "cpu_tests" / "v1" / "31.json"};

const std::filesystem::path POP_BC_tests_path{test_data_dir / "cpu_tests" / "v1" / "C1.json"};
const std::filesystem::path POP_DE_tests_path{test_data_dir / "cpu_tests" / "v1" / "D1.json"};
const std::filesystem::path POP_HL_tests_path{test_data_dir / "cpu_tests" / "v1" / "E1.json"};
const std::filesystem::path POP_AF_tests_path{test_data_dir / "cpu_tests" / "v1" / "F1.json"};

const std::filesystem::path PUSH_BC_tests_path{test_data_dir / "cpu_tests" / "v1" / "C5.json"};
const std::filesystem::path PUSH_DE_tests_path{test_data_dir / "cpu_tests" / "v1" / "D5.json"};
const std::filesystem::path PUSH_HL_tests_path{test_data_dir / "cpu_tests" / "v1" / "E5.json"};
const std::filesystem::path PUSH_AF_tests_path{test_data_dir / "cpu_tests" / "v1" / "F5.json"};

const std::filesystem::path LD_HL_SP_e8_tests_path{test_data_dir / "cpu_tests" / "v1" / "F8.json"};
const std::filesystem::path LD_Ia16I_SP_tests_path{test_data_dir / "cpu_tests" / "v1" / "08.json"};
const std::filesystem::path LD_SP_HL_tests_path{test_data_dir / "cpu_tests" / "v1" / "F9.json"};
} // namespace

TEST(test_cpu_BIG, LD_BC_n16)
{
    auto const all_data = read_cpu_data(LD_BC_n16_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_DE_n16)
{
    auto const all_data = read_cpu_data(LD_DE_n16_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_HL_n16)
{
    auto const all_data = read_cpu_data(LD_HL_n16_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_SP_n16)
{
    auto const all_data = read_cpu_data(LD_SP_n16_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, POP_BC)
{
    auto const all_data = read_cpu_data(POP_BC_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, POP_DE)
{
    auto const all_data = read_cpu_data(POP_DE_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, POP_HL)
{
    auto const all_data = read_cpu_data(POP_HL_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, POP_AF)
{
    auto const all_data = read_cpu_data(POP_AF_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, PUSH_BC)
{
    auto const all_data = read_cpu_data(PUSH_BC_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, PUSH_DE)
{
    auto const all_data = read_cpu_data(PUSH_DE_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, PUSH_HL)
{
    auto const all_data = read_cpu_data(PUSH_HL_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, PUSH_AF)
{
    auto const all_data = read_cpu_data(PUSH_AF_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_HL_SP_e8)
{
    auto const all_data = read_cpu_data(LD_HL_SP_e8_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_Ia16I_SP)
{
    auto const all_data = read_cpu_data(LD_Ia16I_SP_tests_path);
    validate_cpu_states(all_data);
}

TEST(test_cpu_BIG, LD_SP_HL)
{
    auto const all_data = read_cpu_data(LD_SP_HL_tests_path);
    validate_cpu_states(all_data);
}