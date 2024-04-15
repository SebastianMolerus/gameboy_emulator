#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>

const std::filesystem::path boot_rom_file{BOOT_ROM_FILE};
const std::filesystem::path rom_file{ROM_FILE};

std::vector<uint8_t> read_file(const std::filesystem::path file_path, int size = std::numeric_limits<int>::max())
{
    assert(std::filesystem::exists(file_path));
    std::ifstream ifs{file_path, std::ios_base::in | std::ios_base::binary};
    assert(ifs.is_open());

    ifs.seekg(0, ifs.end);
    int const file_size = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    std::vector<uint8_t> result(std::min(file_size, size), {});
    ifs.read(reinterpret_cast<char *>(result.data()), result.size());
    return result;
}

std::array<uint8_t, 256> load_boot_rom()
{
    std::vector<uint8_t> file_content{read_file(boot_rom_file, 256)};
    std::array<uint8_t, 256> result;
    std::copy(file_content.begin(), file_content.end(), result.begin());
    return result;
}

std::vector<uint8_t> load_rom()
{
    return read_file(rom_file);
}