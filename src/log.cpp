#include "log.hpp"
#include <fstream>

namespace sm
{

log::log(std::string file_name) : file{std::move(file_name)}
{
}

void log::save(std::span<uint8_t> data)
{
    std::ofstream ofs{file, std::ofstream::out | std::ofstream::binary};
    for (auto &d : data)
        ofs << d;
}

} // namespace sm
