
******
HDF5pp
******

.. |badge1| image:: https://img.shields.io/badge/license-MIT-brightgreen.svg
  :target: https://github.com/tdegeus/HDF5pp/blob/master/LICENSE
  :alt: MIT license

.. |badge2| image:: https://img.shields.io/badge/warranty-no-red.svg
  :target: https://github.com/tdegeus/HDF5pp/blob/master/LICENSE
  :alt: MIT license

.. |badge3| image:: https://img.shields.io/badge/download-.zip-lightgray.svg
  :target: https://github.com/tdegeus/HDF5pp/zipball/master
  :alt: Download as .zip

.. |badge4| image:: https://img.shields.io/badge/download-.tar.gz-lightgray.svg
  :target: https://github.com/tdegeus/HDF5pp/tarball/master
  :alt: Download as .tar.gz

.. |badge5| image:: https://img.shields.io/badge/contact-tom@geus.me-blue.svg
  :target: mailto:tom@geus.me
  :alt: Contact tom@geus.me

.. |badge6| image:: https://img.shields.io/badge/contact-www.geus.me-blue.svg
  :target: http://www.geus.me
  :alt: Website www.geus.me

.. |badge7| image:: https://img.shields.io/badge/GitHub-tdegeus/HDF5pp-blue.svg
  :target: https://github.com/tdegeus/HDF5pp
  :alt: Github tdegeus/HDF5pp

.. |badge8| image:: https://img.shields.io/badge/documentation-HDF5pp.geus.me-blue.svg
  :target: http://HDF5pp.geus.me
  :alt: Website HDF5pp.geus.me

| |badge1| |badge2| |badge3| |badge4|
| |badge5| |badge6| |badge7|
| |badge8|

.. note::

  This library is free to use under the `MIT license <https://github.com/tdegeus/HDF5pp/blob/master/LICENSE>`_. Any additions are very much appreciated, in terms of suggested functionality, code, documentation, testimonials, word of mouth advertisement, .... Bug reports or feature requests can be filed on `GitHub <http://github.com/tdegeus/HDF5pp>`_. As always, the code comes with no guarantee. None of the developers can be held responsible for possible mistakes.

Teaser
======

This header-only module provides a very simple interface to store data to a HDF5-file. Using this library writing an HDF5 file becomes as simple as:

.. code-block:: cpp

  #include <iostream>
  #include <vector>
  #include <HDF5pp.h>

  int main()
  {
    std::vector<double> data = ...;

    H5p::File file = H5p::File("example.hdf5","w");

    file.write("/path/to/data", data);

    return 0;
  }

.. note::

  Compilation of this example for example with

  .. code-block:: bash

    h5c++ -std=c++14 `pkg-config --cflags HDF5pp` example.cpp

  But probably most easily using CMake. See :ref:`compile` for more details.

.. note::

  In addition to the header-only library this module provides some :ref:`command-line tools <tools>`.

.. note::

  It might be interesting to look at the header-only library [cpppath](https://github.com/tdegeus/cpppath) to handle the paths of the HDF5 datasets.

Contents
========

.. toctree::
   :maxdepth: 1

   usage.rst
   compile.rst
   tools.rst
   develop.rst
