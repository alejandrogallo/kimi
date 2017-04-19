
#ifndef VERSION
#define VERSION "no-defined"
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

// libint
#include <libint2.hpp>

#include <clibs/debug/debug.h>
#include <clibs/inih/cpp/INIReader.h>

static const std::string XYZ_INPUT_NAME = "input.xyz";
static const std::string CONFIG_FILE_NAME = "config.ini";

INIReader get_configuration(std::string file_name = CONFIG_FILE_NAME)
{
    INIReader reader(file_name);
    if (reader.ParseError() < 0) {
        std::cout << "Can't load " << file_name << std::endl;
        return reader;
    }
    return reader;
}

int main(int argc, char* argv[]) {

  std::cout << "VERSION: " << VERSION << std::endl;
  INIReader configuration = get_configuration();
  Debug logger("[main]", Debug::verbose, std::cout);
  const std::string structure_file = configuration.Get("ini", "structure", XYZ_INPUT_NAME);
  const std::string basis_set = configuration.Get("ini", "basis-set", XYZ_INPUT_NAME);


  logger << logger.info << "Config loaded from = " << CONFIG_FILE_NAME << std::endl
                        << "Structure file     = " << structure_file   << std::endl
                        << "Basis set          = " << basis_set        << std::endl;

  logger << logger.info << "Initialising libint2" << std::endl;
  libint2::initialize();  // safe to use libint now

  // all other code snippets go here
  logger << logger.info << "Reading in input file " << structure_file << std::endl;
  std::ifstream input_file(structure_file);
  std::vector<libint2::Atom> atoms = libint2::read_dotxyz(input_file);

  logger << logger.info << "Initializing basis set" << std::endl;
  libint2::BasisSet orbitals(basis_set, atoms);

  for (unsigned i ; i < orbitals.size() ; i++) {
    //std::cout << orbitals[i] << std::endl;
    //int p = orbitals[i].cartesian_size();
    //std::cout << sizeof(p) << std::endl;
    std::cout << i << std::endl;
    std::cout << i << std::endl;
    //std::cout << p << std::endl;
    //std::cout << orbitals[i].cartesian_size() << std::endl;
    //std::cout << orbitals[i].O << std::endl;
  }


  //std::copy(
    //begin(orbitals),
    //end(orbitals),
    //std::ostream_iterator<libint2::Shell>(std::cout, "\n")
   //);


  libint2::finalize();  // do not use libint after this
  // can repeat the libint2::initialize() ... finalize() cycle as many times as
  // necessary

  return 0;
}

