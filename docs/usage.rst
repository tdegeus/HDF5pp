
.. _usage:

*****
Usage
*****

Example using Eigen
===================

[:download:`source: examples/eigen/example.cpp <examples/eigen/example.cpp>`]

[:download:`source: examples/eigen/CMakeLists.txt <examples/eigen/CMakeLists.txt>`]

Writing arrays can be done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <Eigen/Eigen>
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
    mat_size(2,1) = 1234567891234567891;

    ColS vec_size(4);
    vec_size(0) = 0;
    vec_size(1) = 1;
    vec_size(2) = 2;
    vec_size(3) = 3;

    H5p::File file = H5p::File("example.h5");
    file.write("/mat_double",mat_double);
    file.write("/mat_size"  ,mat_size  );
    file.write("/vec_size"  ,vec_size  );

    return 0;
  }

Then, reading with Python is straightforward:

.. code-block:: python

  import h5py
  import numpy as np

  f = h5py.File('example.h5','r')

  print(f['mat_double'][:])
  print(f['mat_size'  ][:])
  print(f['vec_size'  ][:])

