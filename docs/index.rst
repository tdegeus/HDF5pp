
******
HDF5pp
******

This header-only module provides a very simple interface to store data to a HDF5-file. It's usage, programmatically and from a compilation perspective, is really simple. One just has to include the relevant header file and tell your compiler where it is located (and to the C++14 or younger standard). Really, that's it!

Using this library writing an HDF5 file becomes as simple as:

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

  This library is free to use under the `MIT license <https://github.com/tdegeus/HDF5pp/blob/master/LICENSE>`_. Any additions are very much appreciated, in terms of suggested functionality, code, documentation, testimonials, word of mouth advertisement, .... Bug reports or feature requests can be filed on `GitHub <http://github.com/tdegeus/HDF5pp>`_. As always, the code comes with no guarantee. None of the developers can be held responsible for possible mistakes.

  Download: `.zip file <https://github.com/tdegeus/HDF5pp/zipball/master>`_ | `.tar.gz file <https://github.com/tdegeus/HDF5pp/tarball/master>`_.

  (c - `MIT <https://github.com/tdegeus/HDF5pp/blob/master/LICENSE>`_) T.W.J. de Geus (Tom) | tom@geus.me | `www.geus.me <http://www.geus.me>`_ | `github.com/tdegeus/HDF5pp <http://github.com/tdegeus/HDF5pp>`_

Contents
========

.. toctree::
   :maxdepth: 1

   usage.rst
   compile.rst
   develop.rst

