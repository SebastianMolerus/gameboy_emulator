#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <fstream>
#include <sstream>

namespace sm
{
class log
{
  public:
    log(std::string file_name);
    ~log();
    void save(std::string s);

  private:
    std::string file;
    std::ofstream ofs;
    std::stringstream ss;
};
} // namespace sm

#endif