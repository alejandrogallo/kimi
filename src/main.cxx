
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

#include <constants.h>


INIReader get_configuration(std::string file_name = kimi::CONFIG_FILE_NAME)
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
  const std::string structure_file = configuration.Get("ini", "structure", kimi::XYZ_INPUT_NAME);
  const std::string basis_set = configuration.Get("ini", "basis-set", "sto-3g");


  logger << logger.info << "Config loaded from = " << kimi::CONFIG_FILE_NAME << std::endl
                        << "Structure file     = " << structure_file   << std::endl
                        << "Basis set          = " << basis_set        << std::endl;

  logger << logger.info << "Initialising libint2" << std::endl;
  libint2::initialize();  // safe to use libint now

  // all other code snippets go here
  logger << logger.info << "Reading in input file " << structure_file << std::endl;
  std::ifstream input_file(structure_file);
  std::vector<libint2::Atom> atoms = libint2::read_dotxyz(input_file);

  logger << logger.info << "Initializing basis set" << std::endl;
  libint2::BasisSet shells(basis_set, atoms);

  for (unsigned i ; i < shells.size() ; i++) {
    //std::cout << shells[i] << std::endl;
    int csize = shells[i].cartesian_size();
    int size = shells[i].size();
    std::cout << "Shell " << i << std::endl;
    std::cout << "  Cartesian size " << csize << std::endl;
    std::cout << "  Size " << size << std::endl;
    std::cout << "  Origin (Bohr) "
              << shells[i].O[0]*kimi::bohr_to_angstrom << " "
              << shells[i].O[1]*kimi::bohr_to_angstrom << " "
              << shells[i].O[2]*kimi::bohr_to_angstrom << std::endl;
    //std::cout << p << std::endl;
    //std::cout << shells[i].cartesian_size() << std::endl;
    //std::cout << shells[i].O << std::endl;
  }


  //std::copy(
    //begin(shells),
    //end(shells),
    //std::ostream_iterator<libint2::Shell>(std::cout, "\n")
   //);


  libint2::finalize();  // do not use libint after this
  // can repeat the libint2::initialize() ... finalize() cycle as many times as
  // necessary

  return 0;
}

