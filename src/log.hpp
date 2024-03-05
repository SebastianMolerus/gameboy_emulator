#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <span>

namespace sm
{
class log
{
  public:
    log(std::string file_name);

    void save(std::span<uint8_t> data);

  private:
    std::string file;
};
} // namespace sm

#endif