
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

#include "constants.h"
#include "config.h"
#include "utils.h"
#include "algorithms/HartreeFock.h"



int main(int argc, char* argv[]) {

  std::cout << "VERSION: " << VERSION << std::endl;
  const std::string configuration_file = (argc > 1) ? argv[1] : kimi::CONFIG_FILE_NAME ;
  INIReader configuration = kimi::get_configuration(configuration_file);
  Debug logger("[main]", Debug::verbose, std::cout);
  const std::string structure_file = configuration.Get("ini",
      "structure", kimi::XYZ_INPUT_NAME);
  const std::string basis_set = configuration.Get("ini",
      "basis-set", "sto-3g");
  int number_of_electrons = 0;
  int i;
  int n_basis_functions;
  int electronic_iterations = configuration.GetInteger("ini",
      "electronic-iterations", 100);
  double electronic_convergence = configuration.GetReal("ini",
      "electronic-convergence", 1e-12);



  logger << logger.info << "Config loaded from     = "
                        << configuration_file     << std::endl
                        << "Structure file         = "
                        << structure_file         << std::endl
                        << "Electronic iterations  = "
                        << electronic_iterations  << std::endl
                        << "Electronic convergence = "
                        << electronic_convergence << std::endl
                        << "Basis set              = "
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
            << kimi::compute_nuclear_repulsion_energy(atoms) << std::endl;

  logger << "Number of electrons = " << number_of_electrons << std::endl;

  logger << logger.info << "Initializing basis set" << std::endl;
  libint2::BasisSet shells(basis_set, atoms);

  n_basis_functions = shells.nbf();

  logger << logger.info
         << "Number of basis functions = "
         << n_basis_functions
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
  "          _                        " << std::endl <<
  "         /      *               3  " << std::endl <<
  " S   =   |   phi  (r) phi  (r) d  r" << std::endl <<
  "  ij    _/      i        j         " << std::endl;

  Eigen::MatrixXd S = kimi::compute_1body_ints(
      shells, libint2::Operator::overlap);
  std::cout << S << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Calculating kinteic integrals" << std::endl;
  std::cout <<
  "          _                             " << std::endl <<
  "         /      *                    3  " << std::endl <<
  " T   =   |   phi  (r) grad phi  (r) d  r" << std::endl <<
  "  ij    _/      i             j         " << std::endl;

  Eigen::MatrixXd T = kimi::compute_1body_ints(
      shells, libint2::Operator::kinetic);
  std::cout << T << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Calculating kinteic integrals" << std::endl;
  std::cout <<
  "          _                                     " << std::endl <<
  "         /      *      __    1               3  " << std::endl <<
  " V   =   |   phi  (r) \\   -------- phi  (r) d  r" << std::endl <<
  "  ij    _/      i     /__ | R - r|    j         " << std::endl <<
  "                         v   v                  " << std::endl;

  Eigen::MatrixXd V = kimi::compute_1body_ints(
      shells, libint2::Operator::nuclear, atoms);
  std::cout << V << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Calculating the core hamiltonian" << std::endl
            << "  H = T + V" << std::endl;
  Eigen::MatrixXd H = T + V;
  std::cout << H << std::endl;

  // T and V no longer needed, free up the memory
  T.resize(0,0);
  V.resize(0,0);


  Eigen::MatrixXd D(n_basis_functions, n_basis_functions);
  Eigen::MatrixXd D_last = D;

  for ( i=0 ; i < n_basis_functions ; i++) {
    for (unsigned j=0 ; j < n_basis_functions ; j++) {
      D(i,j) = 1;
    }
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Initial Density Matrix:" << std::endl;
  std::cout << D << std::endl;

  int iter = 0;
  double rmsd = 1;
  double energy_diff = 1;
  double ehf = 0;

  double ehf_last = 0;

  do {
    const auto tstart = std::chrono::high_resolution_clock::now();
    ++iter;







    const auto tstop = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> time_elapsed = tstop - tstart;
    if (iter == 1)
      std::cout <<
        "Iter             "
        "E(elec)          "
        "E(tot)           "
        "Delta(E)         "
        "RMS(D)           "
        "Time(s)          " << std::endl;
    printf(" %02d %20.12f %20.12f %20.12f %20.12f %10.5lf\n",
            iter, 0.0, 0.0 + 2.0, energy_diff, rmsd, time_elapsed.count());

  } while (
      ((fabs(energy_diff) > electronic_convergence) ||
      (fabs(rmsd) > electronic_convergence))        &&
      (iter < electronic_iterations)
    );



  libint2::finalize();  // do not use libint after this

  return 0;
}



