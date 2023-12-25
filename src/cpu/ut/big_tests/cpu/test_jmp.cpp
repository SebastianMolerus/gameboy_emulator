#include "../big_tests_utils.h"

namespace
{
std::array<uint8_t, 30> const jump_opcodes{0x20, 0x30, 0x18, 0x28, 0x38, 0xC0, 0xD0, 0xC2, 0xD2, 0xC3,
                                           0xC4, 0xD4, 0xC7, 0xD7, 0xE7, 0xF7, 0xC8, 0xD8, 0xC9, 0xD9,
                                           0xE9, 0xCA, 0xDA, 0xCC, 0xDC, 0xCD, 0xCF, 0xDF, 0xEF, 0xFF};

} // namespace

TEST(test_cpu_BIG, test_all_jump)
{
    for (auto hex : jump_opcodes)
    {
        std::stringstream ss;
        ss << std::hex << static_cast<int>(hex);
        std::cout << "Testing Opcode [0x" << ss.str() << "]\n";
        auto file = ss.str() + ".json";
        const std::filesystem::path test_path{test_data_dir / "cpu_tests" / "v1" / file.c_str()};
        auto const all_data = read_cpu_data(test_path);
        validate_cpu_states(all_data);
    }
}