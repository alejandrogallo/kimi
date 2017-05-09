#include "utils.h"

double
kimi::compute_nuclear_repulsion_energy(std::vector<libint2::Atom>& structure)
{
  int i, j;
  double enuc(0.0), r2(0.0);
  std::cout << "Calculating nuclear repulsion energy" << std::endl;
  for (i = 0; i < structure.size(); ++i) {
    for (j = i + 1; j < structure.size(); ++j) {
      r2 = 0.0;
      std::cout << "Atom_" << i << " , Atom_"<< j << std::endl;
      r2 += pow(structure[i].x - structure[j].x, 2);
      r2 += pow(structure[i].y - structure[j].y, 2);
      r2 += pow(structure[i].z - structure[j].z, 2);
      enuc += structure[i].atomic_number * structure[j].atomic_number
               / sqrt(r2);
    }
  }
  return enuc;
}

Eigen::MatrixXd
kimi::compute_1body_ints(
  const libint2::BasisSet shells,
  libint2::Operator obtype,
  const std::vector<libint2::Atom>& atoms
  )
{

  // Get number of basis set functions
  int n = shells.nbf();
  Eigen::MatrixXd result(n,n);

  // construct the overlap integrals engine
  libint2::Engine engine(obtype, shells.max_nprim(), shells.max_l(), 0);
  // nuclear attraction ints engine needs to know where the charges sit ...
  // the nuclei are charges in this case; in QM/MM there will also be classical charges
  if (obtype == libint2::Operator::nuclear) {
    std::vector<std::pair<double,std::array<double,3>>> q;
    for(const libint2::Atom& atom : atoms) {
      q.push_back(
          {static_cast<double>(atom.atomic_number), {{atom.x, atom.y, atom.z}}}
          );
    }
    engine.set_params(q);
  }

  const std::vector<size_t> shell2bf = shells.shell2bf();

  // buf[0] points to the target shell set after every call  to
  // engine.compute()
  const auto& buf = engine.results();

  // loop over unique shell pairs, {s1,s2} such that s1 >= s2
  // this is due to the permutational symmetry of the real integrals over
  // Hermitian operators: (1|2) = (2|1)
  for(auto s1=0; s1!=shells.size(); ++s1) {

    auto bf1 = shell2bf[s1]; // first basis function in this shell
    auto n1 = shells[s1].size();

    for(auto s2=0; s2<=s1; ++s2) {

      auto bf2 = shell2bf[s2];
      auto n2 = shells[s2].size();

      // compute shell pair; return is the pointer to the buffer
      engine.compute(shells[s1], shells[s2]);

      // "map" buffer to a const Eigen Matrix, and copy it to the corresponding
      // blocks of the result
      Eigen::Map<const Eigen::MatrixXd> buf_mat(buf[0], n1, n2);
      result.block(bf1, bf2, n1, n2) = buf_mat;
      if (s1 != s2) // if s1 >= s2, copy {s1,s2} to the corresponding {s2,s1}
      //block, note the transpose!
      result.block(bf2, bf1, n2, n1) = buf_mat.transpose();

    }
  }

  return result;
}

Eigen::MatrixXd
kimi::compute_soad(const std::vector<libint2::Atom>& atoms) {

  // compute number of atomic orbitals
  size_t nao = 0;
  for(const auto& atom: atoms) {
    const auto Z = atom.atomic_number;
    if (Z == 1 || Z == 2) // H, He
      nao += 1;
    else if (Z <= 10) // Li - Ne
      nao += 5;
    else
      throw "SOAD with Z > 10 is not yet supported";
  }

  // compute the minimal basis density
  Eigen::MatrixXd D = Eigen::MatrixXd::Zero(nao, nao);
  size_t ao_offset = 0; // first AO of this atom
  for(const auto& atom: atoms) {
    const auto Z = atom.atomic_number;
    if (Z == 1 || Z == 2) { // H, He
      D(ao_offset, ao_offset) = Z; // all electrons go to the 1s
      ao_offset += 1;
    }
    else if (Z <= 10) {
      D(ao_offset, ao_offset) = 2; // 2 electrons go to the 1s
      D(ao_offset+1, ao_offset+1) = (Z == 3) ? 1 : 2; // Li? only 1 electron in 2s, else 2 electrons
      // smear the remaining electrons in 2p orbitals
      const double num_electrons_per_2p = (Z > 4) ? (double)(Z - 4)/3 : 0;
      for(auto xyz=0; xyz!=3; ++xyz)
        D(ao_offset+2+xyz, ao_offset+2+xyz) = num_electrons_per_2p;
      ao_offset += 5;
    }
  }

  return D * 0.5; // we use densities normalized to # of electrons/2
}


