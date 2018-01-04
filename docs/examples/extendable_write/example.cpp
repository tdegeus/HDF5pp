
#include <iostream>
#include <vector>
#include <HDF5pp.h>


int main()
{
  H5p::File file = H5p::File("example.hdf5","w");


  for ( size_t i = 0 ; i < 10 ; ++i )
  {
    double data = static_cast<double>(i * 2) + 0.1;

    std::cout << "index = " << i << std::endl;

    file.write("/data",data,i);
  }



  return 0;
}
