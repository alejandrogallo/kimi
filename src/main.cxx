
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

#include "constants.h"
#include "utils.h"


INIReader
get_configuration(std::string file_name)
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
  const std::string configuration_file = (argc > 1) ? argv[1] : kimi::CONFIG_FILE_NAME ;
  INIReader configuration = get_configuration(configuration_file);
  Debug logger("[main]", Debug::verbose, std::cout);
  const std::string structure_file = configuration.Get("ini",
      "structure", kimi::XYZ_INPUT_NAME);
  const std::string basis_set = configuration.Get("ini",
      "basis-set", "sto-3g");
  int number_of_electrons = 0;
  int i;


  logger << logger.info << "Config loaded from = "
                        << configuration_file     << std::endl
                        << "Structure file     = "
                        << structure_file         << std::endl
                        << "Basis set          = "
                        << basis_set              << std::endl;

  logger << logger.info << "Initialising libint2" << std::endl;
  libint2::initialize();  // safe to use libint now

  // all other code snippets go here
  logger << logger.info
         << "Reading in input file "
         << structure_file
         << std::endl;
  std::ifstream input_file(structure_file);
  std::vector<libint2::Atom> atoms = libint2::read_dotxyz(input_file);
  logger << "Number of atoms = " << atoms.size() << std::endl;

  // Compute number of electrons
  for (i = 0; i < atoms.size(); ++i) {
    number_of_electrons += atoms[i].atomic_number;
  }
  std::cout << "Nuclear repulsion energy "
            << compute_nuclear_repulsion_energy(atoms) << std::endl;

  logger << "Number of electrons = " << number_of_electrons << std::endl;

  logger << logger.info << "Initializing basis set" << std::endl;
  libint2::BasisSet shells(basis_set, atoms);

  logger << logger.info
         << "Number of basis functions = "
         << shells.nbf()
         << std::endl;
  logger << logger.info
         << "Maximum number of primitives in shells = "
         << shells.max_nprim()
         << std::endl;
  logger << logger.info
         << "Maximum value of l in shells = "
         << shells.max_l()
         << std::endl;

  for (unsigned i ; i < shells.size() ; i++) {
    std::cout << shells[i] << std::endl;
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Calculating overlaps" << std::endl;

  std::cout <<
  "  _                        " << std::endl <<
  " /      *               3  " << std::endl <<
  " |   phi  (r) phi  (r) d  r" << std::endl <<
  "_/      i        j         " << std::endl;

  libint2::Engine overlap_engine(
    libint2::Operator::overlap,
    shells.max_nprim(),
    shells.max_l()
  );

  auto shell2bf = shells.shell2bf();

  // This pointer will point to computed shell sets
  const auto& overlap_buffer = overlap_engine.results();

  for (auto s1 = 0; s1 != shells.size(); ++s1) {
    for (auto s2 = 0; s2 != shells.size(); ++s2) {

      std::cout << "Compute shell set {" << s1 << "," << s2 << "} ... ";
      overlap_engine.compute(shells[s1], shells[s2]);
      std::cout << "Done" << std::endl;

      // Location of the computed integrals
      auto ints_shellset = overlap_buffer[0];
      if (ints_shellset == nullptr) {
        // nullptr if the entire shell-set was screened out
        continue;
      }

    }
  }







  libint2::finalize();  // do not use libint after this
  // can repeat the libint2::initialize() ... finalize() cycle as many times as
  // necessary

  return 0;
}

//Matrix compute_1body_ints(
  //const std::vector<libint2::Shell>& shells,
  //libint2::Operator obtype,
  //const std::vector<Atom>& atoms)
//{
  //using libint2::Shell;
  //using libint2::Engine;
  //using libint2::Operator;

  //const auto n = nbasis(shells);
  //Matrix result(n,n);

  //// construct the overlap integrals engine
  //Engine engine(obtype, max_nprim(shells), max_l(shells), 0);
  //// nuclear attraction ints engine needs to know where the charges sit ...
  //// the nuclei are charges in this case; in QM/MM there will also be classical charges
  //if (obtype == Operator::nuclear) {
    //std::vector<std::pair<double,std::array<double,3>>> q;
    //for(const auto& atom : atoms) {
      //q.push_back( {static_cast<double>(atom.atomic_number), {{atom.x, atom.y, atom.z}}} );
    //}
    //engine.set_params(q);
  //}

  //auto shell2bf = map_shell_to_basis_function(shells);

  //// buf[0] points to the target shell set after every call  to engine.compute()
  //const auto& buf = engine.results();

  //// loop over unique shell pairs, {s1,s2} such that s1 >= s2
  //// this is due to the permutational symmetry of the real integrals over Hermitian operators: (1|2) = (2|1)
  //for(auto s1=0; s1!=shells.size(); ++s1) {

    //auto bf1 = shell2bf[s1]; // first basis function in this shell
    //auto n1 = shells[s1].size();

    //for(auto s2=0; s2<=s1; ++s2) {

      //auto bf2 = shell2bf[s2];
      //auto n2 = shells[s2].size();

      //// compute shell pair; return is the pointer to the buffer
      //engine.compute(shells[s1], shells[s2]);

      //// "map" buffer to a const Eigen Matrix, and copy it to the corresponding blocks of the result
      //Eigen::Map<const Matrix> buf_mat(buf[0], n1, n2);
      //result.block(bf1, bf2, n1, n2) = buf_mat;
      //if (s1 != s2) // if s1 >= s2, copy {s1,s2} to the corresponding {s2,s1} block, note the transpose!
      //result.block(bf2, bf1, n2, n1) = buf_mat.transpose();

    //}
  //}

  //return result;
//}


