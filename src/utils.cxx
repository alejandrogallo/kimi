#include "utils.h"

double
compute_nuclear_repulsion_energy(std::vector<libint2::Atom>& structure)
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
