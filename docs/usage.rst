
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
    std::vector<double> data = ...;

    H5p::File file = H5p::File("example.hdf5", "w");

    file.write("/path/to/data", data);

    std::vector<double> read = file.read<std::vector<double>>("/path/to/data");

    return 0;
  }

.. note::

  Although this library is header only, the HDF5 library should be linked. Therefore using either ``h5c++`` or CMake can be used, see :ref:`compile`.

Function overview
=================

All functions are members of the File class:

.. code-block:: cpp

  H5p::File("/path/to/file", "mode");

The constructor takes two arguments: the file name and the read/write mode. For the latter there are three possibilities:

- ``"w"``: write a new file or overwrite existing file.
- ``"r"``: read from existing file.
- ``"r+"`` or ``"a"``: read from and write to an existing file.

In addition it takes one option, the flush settings. The default ``true`` ensures the file to be flushed after each write operation, allowing external reading while the file is open.

Main functions:

* ``void File::write("/path/to/data",...)``

  Write data (scalar, array, matrix, ...). Can be overloaded with many different types, see :ref:`overloaded_types`.

* ``void File::overwrite("/path/to/data",...)``

  Overwrite data (scalar, array, matrix, ...) of an existing datasets. Can be overloaded with many different types, see :ref:`overloaded_types`. Note that the type and shape must match the existing dataset.

* ``Type File::read<Type>("/path/to/data")``

  Read data (scalar, array, matrix, ...). Can be templated with many different types, see :ref:`overloaded_types`.

* ``std::vector<size_t> shape("/path/to/data")``

  Return the shape of the data array.

* ``size_t shape("/path/to/data", i)``

  Return the shape of the data array along axis ``i``.

* ``size_t size("/path/to/data")``

  Return the number of elements in the data array.

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

* ``int``
* ``size_t``
* ``float``
* ``double``
* ``std::string``

Writing and or reading is done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    double data = 10.;

    H5p::File file = H5p::File("example.hdf5", "w");

    file.write("/path/to/data", data);

    double read_data = file.read<double>("/path/to/data");

    return 0;
  }

[:download:`source: example.cpp <examples/scalar/example.cpp>`, :download:`compile: CMakeLists.txt <examples/scalar/CMakeLists.txt>`]

Basic types, part of an expandable array (size_t, double, ...)
--------------------------------------------------------------

In this case the scalar will be part of an array that automatically expands to contain new entries. The behavior is thus like allocating an array of arbitrary shape and then filling it item-by-item. The actual size is determined by the highest index specified. All entries in the array that have not been explicitly specified are assigned a default fill value. Note:

* One can read one value from, but also read the array as any array (i.e. using ``file.read<std::vector<...>>(...)``).

* One can convince oneself about the size of the array using the standard tools (``file.size(...)`` and ``file.shape(...)``).

* At the first call the array some properties of the array are defined. At this time can choose the fill value (``fill_val``) and the size of the blocks in which the array is stored in the file (``chunk_size``). If one knows the ultimate size one can store in one chunk (most efficient). Otherwise one should choose a value which is high enough not to get a very scattered file, but low enough not to allocate a lot of space that is not used.

The examples below feature a ``double``, which may be replaced with:

* ``int``
* ``size_t``
* ``float``
* ``double``

Writing and or reading is done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.hdf5", "w");

    double data = 10.;
    size_t idx  = 0;

    file.write("/path/to/data", data, idx);

    data = 20.;
    idx  = 1;

    // "/path/to/data" is automatically expanded to contain the new entry
    file.write("/path/to/data", data, idx);

    // read one entry
    idx = 0;
    double read_entry = file.read<double>("/path/to/data", idx);

    // read entire array
    std::vector<double> read_data = file.read<std::vector<double>>("/path/to/data");

    return 0;
  }

[:download:`source: example.cpp <examples/scalar/example.cpp>`, :download:`compile: CMakeLists.txt <examples/scalar/CMakeLists.txt>`]

std::vector
-----------

Writing a vector (and optionally its 'dimensions') is done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    H5p::File file = H5p::File("example.hdf5", "w");

    std::vector<double> data  = { 0., 1., 2., 3., 4., 5. };
    std::vector<size_t> shape = { 3 , 2 };

    file.write("/path/to/data", data, shape);

    std::vector<double> read_data  = file.read<std::vector<double>>("/path/to/data");
    std::vector<size_t> read_shape = file.shape("/path/to/data");

    return 0;
  }

[:download:`source: example.cpp <examples/vector/example.cpp>`, :download:`compile: CMakeLists.txt <examples/vector/CMakeLists.txt>`]

.. note::

  In the HDF5 archive the data is stored as a matrix. However, because ``std::vector`` is just an array the shape has to be extracted separately. For the richer classes below this is not necessary.

Reading with Python does allow direct interpretation of the matrix

.. code-block:: python

  import h5py
  import numpy as np

  f = h5py.File('example.hdf5','r')

  print(f['/data'][...])

[:download:`source: example.py <examples/vector/example.py>`]

cppmat - multidimensional arrays
--------------------------------

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

Writing and reading matrices of arbitrary dimensions can be done as follows:

.. code-block:: cpp

  #include <iostream>
  #include <cppmat/cppmat.h>
  #include <HDF5pp.h>

  int main()
  {
    cppmat::array<double> data({2,3,4,5});

    // ... fill "data"

    H5p::File file = H5p::File("example.hdf5", "w");

    file.write("/path/to/data", data);

    cppmat::array<double> read_data = file.read<cppmat::array<double>>("/path/to/data");

    return 0;
  }

[:download:`source: example.cpp <examples/cppmat/example.cpp>`, :download:`compile: CMakeLists.txt <examples/cppmat/CMakeLists.txt>`]

.. note::

  Several other cppmat-classes can be read directly using for example:

  .. code-block:: cpp

    cppmat::matrix<double> read_data = file.read<cppmat::array<double>>("/path/to/data");

  Whether or not your class can be read depends cppmat (if there exists some automatic conversion, which is the case for most classes but not for all).

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

Writing and reading matrices or arrays can be done as follows:

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

    H5p::File file = H5p::File("example.hdf5", "w");

    file.write("/path/to/data", data);

    MatD read_data = file.read<MatD>("/path/to/data");

    return 0;
  }

[:download:`source: example.cpp <examples/eigen/example.cpp>`, :download:`compile: CMakeLists.txt <examples/eigen/CMakeLists.txt>`]
