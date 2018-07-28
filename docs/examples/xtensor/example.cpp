
#include <iostream>

// #define HDF5PP_XTENSOR

#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <HDF5pp.h>

int main()
{
  // open empty file
  H5p::File file = H5p::File("example.hdf5", "w");

  // some example data
  xt::xarray<double> data = xt::arange<double>(2*3*4); data.reshape({2,3,4});

  // write data
  file.write("/data", data);

  // read data
  xt::xtensor<double,3> foo = file.xread<double,3>("/data");

  // print for verification
  std::cout << foo << std::endl;

  return 0;
}
