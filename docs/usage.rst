
.. _usage:

*****
Usage
*****

std::vector
===========

Writing the array (and optionally it's 'dimensions') is done as follows:

.. code-block:: cpp

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

[:download:`source: example.cpp <examples/vector_write/example.cpp>`, :download:`compile: CMakeLists.txt <examples/vector_write/CMakeLists.txt>`]

Reading is also straightforward:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    std::vector<size_t> shape = file.shape("/data");
    std::vector<double> data  = file.read<double>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/vector_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/vector_read/CMakeLists.txt>`]

Reading with Python does allow direct interpretation of the matrix

.. code-block:: python

  import h5py
  import numpy as np

  f = h5py.File('example.h5','r')

  print(f['data'][:])

[:download:`source: example.py <examples/vector_write/example.py>`]

cppmat multi-dimensional arrays
===============================

To enable this feature:

*   Include cppmat before HDF5pp:

    .. code-block:: cpp

      #include <cppmat/cppmat.h>
      #include <HDF5pp.h>

*   Define ``HDF5PP_CPPMAT`` somewhere before including HDF5pp:

    .. code-block:: cpp

      #define HDF5PP_CPPMAT
      #include <HDF5pp.h>
      #include <cppmat/cppmat.h>

Writing matrices of arbitrary dimensions can be done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    cppmat::matrix<double> data({2,3,4,5});

    // ... fill "data"

    H5p::File file = H5p::File("example.h5","w");

    file.write("/data",data);

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_write/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_write/CMakeLists.txt>`]

Reading is also straightforward:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    cppmat::matrix<double> data = file.read<double>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_read/CMakeLists.txt>`]

Reading with Python is also straightforward:

.. code-block:: python

  import h5py
  import numpy as np

  f = h5py.File('example.h5','r')

  print(f['data'][:])

[:download:`source: example.py <examples/eigen_cppmat_write/example.py>`]

Eigen matrices
==============

To enable this feature:

*   Include Eigen before HDF5pp:

    .. code-block:: cpp

      #include <Eigen/Eigen>
      #include <HDF5pp.h>

*   Define ``HDF5PP_EIGEN`` somewhere before including HDF5pp:

    .. code-block:: cpp

      #define HDF5PP_EIGEN
      #include <HDF5pp.h>
      #include <Eigen/Eigen>

Writing matrices or arrays can be done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <Eigen/Eigen>
  #include <HDF5pp.h>

  // alias row-major Eigen matrix
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatD;

  int main()
  {
    MatD data(2,2);

    // ... fill "data"

    H5p::File file = H5p::File("example.h5","w");

    file.write("/data",data);

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_write/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_write/CMakeLists.txt>`]

Reading requires the use of cppmat, which can copy directly to Eigen:

.. code-block:: cpp

  #include <iostream>
  #include <Eigen/Eigen>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  // alias row-major Eigen matrix
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatD;

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    MatD data = file.read<double>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_read/CMakeLists.txt>`]

.. note::

  Note that the Eigen interface of cppmat is automatically enabled by including it after including Eigen.

Reading with Python is also straightforward:

.. code-block:: python

  import h5py
  import numpy as np

  f = h5py.File('example.h5','r')

  print(f['data'][:])

[:download:`source: example.py <examples/eigen_cppmat_write/example.py>`]
