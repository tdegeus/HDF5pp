
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
  MatD mat_double(2,2);
  mat_double(0,0) = 0;
  mat_double(0,1) = 1;
  mat_double(1,0) = 2;
  mat_double(1,1) = 3;

  MatS mat_size(3,2);
  mat_size(0,0) = 0;
  mat_size(0,1) = 1;
  mat_size(1,0) = 2;
  mat_size(1,1) = 3;
  mat_size(2,0) = 4;
  mat_size(2,1) = 5;

  ColS vec_size(4);
  vec_size(0) = 0;
  vec_size(1) = 1;
  vec_size(2) = 2;
  vec_size(3) = 3;

  cppmat::matrix<double> multidim({2,3,4,5});

  for ( size_t i = 0 ; i < multidim.size() ; ++i )
    multidim[i] = static_cast<double>(i);


  H5p::File file = H5p::File("example.hdf5","w");
  file.write("/mat_double",mat_double);
  file.write("/mat_size"  ,mat_size  );
  file.write("/vec_size"  ,vec_size  );
  file.write("/multidim"  ,multidim  );

  return 0;
}
