
.. _usage:

*****
Usage
*****

General example
===============

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

.. note::

  Although this library is header only, the HDF5 library should be linked. Therefore using either ``h5c++`` or CMake can be used, see :ref:`compile`.

Function overview
=================

All functions are members of the File class:

.. code-block:: cpp

  H5p::File("/path/to/file","mode");

The constructor takes two arguments: the file name and the read/write mode. For the latter there are three possibilities:

- ``"w"``: write a new file or overwrite existing file.
- ``"r"``: read from existing file.
- ``"r+"`` or ``"a"``: read from and write to an existing file.

In addition it takes one option, the flush settings. The default ``true`` ensures the file to be flushed after each write operation, allowing external reading while the file is open.

Main functions:

* ``void File::write("/path/to/data",...)``

  Write data (scalar, array, matrix, ...). Can be overloaded with many different types, see :ref:`overloaded_types`.

* ``Type File::read<Type>("/path/to/data")``

  Read data (scalar, array, matrix, ...). Can be templated with many different types, see :ref:`overloaded_types`.

Support functions:

* ``void File::unlink("/path/to/data")``

  Unlink a path. The dataset is removed when there are no more links to it. Warning: depending on the version of the HDF5 library, the space may not be freed from the file. In that case use ``$ h5repack file1 file2`` to create a new file without the unused data.

* ``bool File::exists("/path/to/data")``

  Check if a path exists.

* ``void File::createGroup("/path/to/group")``

  Create a group. Usually there is no need to call this function because the ``write`` function automatically creates all parent groups.

* ``void File::flush()``

  Flush all buffers associated with a file to disk. Usually there is no need to call this function because the ``write`` function automatically flushes the file (this can be suppressed using the option of the File constructor).

.. _overloaded_types:

Overloaded types
================

.. note::

  If your type of choice is not present please submit an issue on GitHub, or file a pull request.

Basic types (size_t, double, ...)
---------------------------------

The examples below feature a ``double``, which may be replaced with:

* ``size_t``
* ``float``
* ``double``
* ``std::string``

Writing is done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    double data = 10.;

    H5p::File file = H5p::File("example.hdf5","w");

    file.write("/path/to/data",data);

    return 0;
  }

To read:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.h5","r");

    double data = file.read<double>("/data");

    return 0;
  }

std::vector
-----------

Writing a vector (and optionally its 'dimensions') is done as follows:

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
  #include <vector>
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

cppmat - multi-dimensional arrays
---------------------------------

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

Eigen - linear algebra library
------------------------------

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

