
#include <iostream>
#include <vector>
#include <HDF5pp.h>


int main()
{
  // mimic 3x2 vector
  std::vector<double> data  = { 0., 1., 2., 3., 4., 5. };
  std::vector<size_t> shape = { 3 , 2 };

  // write
  H5p::File file = H5p::File("example.hdf5","w");
  file.write("/data",data,shape);

  return 0;
}
