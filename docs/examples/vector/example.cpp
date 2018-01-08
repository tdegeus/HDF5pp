
#include <iostream>
#include <vector>
#include <HDF5pp.h>

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // mimic 3x2 vector
  std::vector<double> data  = { 0., 1., 2., 3., 4., 5. };
  std::vector<size_t> shape = { 3 , 2 };

  // write data
  file.write("/data", data, shape);

  // read data
  std::vector<double> read_data = file.read<std::vector<double>>("/data");

  // print read data
  for ( auto &i : read_data ) std::cout << i << ", ";
  std::cout << std::endl;

  return 0;
}
