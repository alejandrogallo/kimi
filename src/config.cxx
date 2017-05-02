#include "config.h"
#include <iostream>

INIReader
kimi::get_configuration(std::string file_name)
{
  INIReader reader(file_name);
  if (reader.ParseError() < 0) {
    std::cout << "Can't load " << file_name << std::endl;
    return reader;
  }
  return reader;
}
