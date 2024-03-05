#include "log.hpp"
#include <fstream>

namespace sm
{

log::log(std::string file_name) : file{std::move(file_name)}, ofs{file}
{
}

log::~log()
{
    ofs << ss.str();
}

void log::save(std::string s)
{
    ss << s;
}

} // namespace sm
