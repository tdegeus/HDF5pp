
#include <iostream>
#include <Eigen/Eigen>
#include <cppmat/cppmat.h>
#include <HDF5pp.h>

// alias row-major types for <Eigen/Dense>
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatD;
typedef Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatS;
typedef Eigen::Matrix<double, Eigen::Dynamic,              1, Eigen::ColMajor> ColD;
typedef Eigen::Matrix<size_t, Eigen::Dynamic,              1, Eigen::ColMajor> ColS;

int main()
{

  // open file
  H5p::File file = H5p::File("example.hdf5","r");

  // read multidimensional array
  cppmat::matrix<double> multidim = file.read<double>("/multidim");

  std::cout << multidim << std::endl;

  // read matrix -> copy to eigen matrix
  MatD mat_double = file.read<double>("/mat_double");

  std::cout << mat_double << std::endl;

  // read matrix -> copy to eigen matrix
  MatS mat_size = file.read<size_t>("/mat_size");

  std::cout << mat_size << std::endl;

  return 0;
}
