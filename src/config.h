#include <clibs/inih/cpp/INIReader.h>

namespace kimi
{
  typedef INIReader config;

  config
  get_configuration(std::string file_name);
} /* kimi */ 
