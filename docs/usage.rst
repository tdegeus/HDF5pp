
.. _usage:

*****
Usage
*****

The general structure of a program is

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    std::vector<double> out = ...;

    H5p::File file = H5p::File("example.hdf5","w");

    file.write("/path/to/output",out);

    std::vector<double> in = file.read<std::vector<double>>("/path/to/input");

    return 0;
  }

Thereby:

* ``File`` takes two arguments: the name and the mode. For the latter there are three possibilities:

  - ``"w"``: write a new file or overwrite existing file (allows reading).
  - ``"r"``: read from existing file (read-only).
  - ``"a"``: read from and write to an existing file.

* ``File::write("path",...)`` can be overloaded with many different types. If yours is not present please submit a pull request.

* ``File::read<...>("path")`` can be templated with many different types

.. note::

  Although this library is header only, the HDF5 library should be linked. Therefore using either ``h5c++`` or CMake can be used, see :ref:`compile`.

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
    // - without shape
    file.write("/data/as/vector",data);
    // - with shape
    file.write("/data/as/matrix",data,shape);

    return 0;
  }

[:download:`source: example.cpp <examples/vector_write/example.cpp>`, :download:`compile: CMakeLists.txt <examples/vector_write/CMakeLists.txt>`]

To read:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    std::vector<size_t> shape = file.shape("/data");
    std::vector<double> data  = file.read<std::vector<double>>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/vector_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/vector_read/CMakeLists.txt>`]

.. note::

  In the HDF5 archive the data is stored as a matrix. However, because ``std::vector`` is just an array the shape has be extracted separately. For the richer classes below this is not necessary.

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

To read:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    cppmat::matrix<double> data = file.read<cppmat::matrix<double>>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_read/CMakeLists.txt>`]

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

To read:

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

    MatD data = file.read<MatD>("/data");

    return 0;
  }

[:download:`source: example.cpp <examples/eigen_cppmat_read/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen_cppmat_read/CMakeLists.txt>`]

