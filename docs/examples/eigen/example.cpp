
#include <iostream>
#include <Eigen/Eigen>
#include <HDF5pp.h>

// alias row-major types for <Eigen/Dense>
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatD;

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // some example data
  MatD data(2,2);
  data(0,0) = 0.1;
  data(0,1) = 1.1;
  data(1,0) = 2.1;
  data(1,1) = 3.1;

  // write data
  file.write("/data", data);

  // read data
  MatD read_data = file.read<MatD>("/data");

  // print read data
  std::cout << read_data << std::endl;

  return 0;
}
