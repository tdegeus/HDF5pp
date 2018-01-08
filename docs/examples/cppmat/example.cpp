
#include <iostream>
#include <cppmat/cppmat.h>
#include <HDF5pp.h>

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // some example data
  cppmat::matrix<double> data({2,3,4,5});

  for ( size_t i = 0 ; i < data.size() ; ++i )
    data[i] = static_cast<double>(i);

  // write data
  file.write("/data", data);

  // read data
  cppmat::matrix<double> read_data = file.read<cppmat::matrix<double>>("/data");

  // print read data
  std::cout << read_data << std::endl;

  return 0;
}
