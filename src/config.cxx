#include "config.h"
#include <iostream>

kimi::config
kimi::get_configuration(std::string file_name)
{
  kimi::config reader(file_name);
  if (reader.ParseError() < 0) {
    std::cout << "Can't load " << file_name << std::endl;
    return reader;
  }
  return reader;
}
