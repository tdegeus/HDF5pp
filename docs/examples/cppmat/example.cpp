
#include <iostream>
#include <cppmat/cppmat.h>
#include <HDF5pp.h>

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // some example data
  cppmat::array<double> data = cppmat::array<double>::Arange({2,3,4,5});

  // write data
  file.write("/data", data);

  // read data
  cppmat::array<double> read_data = file.read<cppmat::array<double>>("/data");

   // B = read_data;

  // print read data
  std::cout << read_data << std::endl;

  // std::cout << B << std::endl;

  return 0;
}
