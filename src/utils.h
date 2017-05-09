#include <vector>
#include <libint2.hpp>
#include <Eigen/Dense>

namespace kimi
{
  double
  compute_nuclear_repulsion_energy(std::vector<libint2::Atom>& structure);

  Eigen::MatrixXd
  compute_1body_ints(
    const libint2::BasisSet shells,
    libint2::Operator obtype,
    const std::vector<libint2::Atom>& atoms = std::vector<libint2::Atom>()
  );

  Eigen::MatrixXd
  compute_soad(const std::vector<libint2::Atom>& atoms);

} /* kimi */ 

