
#include <iostream>
#include <vector>
#include <HDF5pp.h>


int main()
{
  // write
  H5p::File file = H5p::File("example.hdf5","r");

  // read
  std::vector<size_t> shape = file.shape("/data");
  std::vector<double> data  = file.read<std::vector<double>>("/data");

  // print
  for ( size_t i = 0 ; i < shape[0] ; ++i )
  {
    for ( size_t j = 0 ; j < shape[1] ; ++j )
    {
      std::cout << data[i*shape[1]+j] << ", ";
    }
    std::cout << std::endl;
  }

  return 0;
}
