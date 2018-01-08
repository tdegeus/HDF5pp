
#include <iostream>
#include <vector>
#include <HDF5pp.h>

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // read data in extendable array
  for ( size_t i = 0 ; i < 10 ; ++i )
    file.write("/data", static_cast<double>(i*2)+0.1, i);

  // read data
  std::vector<double> read_data = file.read<std::vector<double>>("/data");

  // print read data
  for ( auto &i : read_data ) std::cout << i << ", ";
  std::cout << std::endl;

  return 0;
}
