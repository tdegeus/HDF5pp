
.. _compile:

*********
Compiling
*********

Introduction
============

This module is header only. So one just has to ``#include <HDF5pp/HDF5pp.h>`` (or only one of the submodules) somewhere in the source code, and to tell the compiler where the header-files are. For the latter, several ways are described below.

One should still link with the HDF5 libraries. This is briefly described in :ref:`linking`.

.. note::

  Before proceeding, some words about optimization. Of course one should use optimization when compiling the release of the code (``-O2`` or ``-O3``). But it is also a good idea to switch of the assertions in the code (mostly checks on size) that facilitate easy debugging, but do cost time. Therefore, include the flag ``-DNDEBUG``. Note that this is all C++ standard. I.e. it should be no surprise, and it is always a good idea to do.

Manual compiler flags
=====================

GNU / Clang
-----------

Add the following compiler's arguments:

.. code-block:: bash

  -I${PATH_TO_HDF5PP}/src -std=c++14

.. note:: **(Not recommended)**

  If you want to avoid separately including the header files using a compiler flag, ``git submodule`` is a nice way to go:

  1.  Include this module as a submodule using

      ``git submodule add https://github.com/tdegeus/HDF5pp.git``.

  2.  Replace the first line of this example by

      ``#include "HDF5pp/src/HDF5pp/HDF5pp.h"``.

      *If you decide to manually copy the header file, you might need to modify this relative path to your liking.*

  Or see :ref:`compile_automatic`. You can also combine the ``git submodule`` with any of the below compiling strategies.

.. _compile_automatic:

(Semi-)Automatic compiler flags
===============================

Install
-------

To enable (semi-)automatic build, one should 'install' HDF5pp somewhere.

Install systemwide (depends on your privileges)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.  Proceed to a (temporary) build directory. For example

    .. code-block:: bash

      $ cd /path/to/HDF5pp/src/build

2.  'Build' HDF5pp. For the path above,

    .. code-block:: bash

      $ cmake ..
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/HDF5pp/src``)

Install in custom location (user)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.  Proceed to a (temporary) build directory. For example

    .. code-block:: bash

      $ cd /path/to/HDF5pp/src/build

2.  'Build' HDF5pp, to install it in a custom location. For the path above,

    .. code-block:: bash

      $ mkdir /custom/install/path
      $ cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/custom/install/path
      $ make install

    (If you've used another build directory, change the first command to ``$ cmake /path/to/HDF5pp/src``)

3.  Add the following path to your ``~/.bashrc`` (or ``~/.zshrc``):

    .. code-block:: bash

      export PKG_CONFIG_PATH=/custom/install/path/share/pkgconfig:$PKG_CONFIG_PATH

.. note:: **(Not recommended)**

  If you do not wish to use ``CMake`` for the installation, or you want to do something custom. You can, of course. Follow these steps:

  1.  Copy the file ``src/HDF5pp.pc.in`` to ``HDF5pp.pc`` to some location that can be found by ``pkg_config`` (for example by adding ``export PKG_CONFIG_PATH=/path/to/HDF5pp.pc:$PKG_CONFIG_PATH`` to the ``.bashrc``).

  2.  Modify the line ``prefix=@CMAKE_INSTALL_PREFIX@`` to ``prefix=/path/to/HDF5pp``.

  3.  Modify the line ``Cflags: -I${prefix}/@INCLUDE_INSTALL_DIR@`` to ``Cflags: -I${prefix}/src``.

  4.  Modify the line ``Version: @HDF5PP_VERSION_NUMBER@`` to reflect the correct release version.

Compiler arguments from 'pkg-config'
------------------------------------

Instead of ``-I...`` one can now use

.. code-block:: bash

  `pkg-config --cflags HDF5pp` -std=c++14

as compiler argument.

Compiler arguments from 'cmake'
-------------------------------

Add the following to your ``CMakeLists.txt``:

.. code-block:: cmake

  set(CMAKE_CXX_STANDARD 14)

  find_package(PkgConfig)

  pkg_check_modules(HDF5PP REQUIRED HDF5pp)
  include_directories(${HDF5PP_INCLUDE_DIRS})

.. _linking:

Linking with the HDF5 libraries
===============================

Using the h5c++ executable
---------------------------

The ``h5c++`` executable provides a wrapper around your compiler, with all flags set correctly to use HDF5. To compile the following suffices:

.. code-block:: bash

  h5c++ `pkg-config --cflags HDF5pp` -std=c++14 example.cpp

Using cmake
-----------

The following basic structure of ``CMakeLists.txt`` can be used:

.. code-block:: cmake

  cmake_minimum_required(VERSION 2.8.12)

  # define a project name
  project(example)

  # define empty list of libraries to link
  set(PROJECT_LIBS "")

  # enforce the C++ standard
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  # set optimization level and switch of assertions (set to your liking)
  set(CMAKE_BUILD_TYPE Release)
  add_definitions(-DNDEBUG)

  # load pkg-config
  find_package(PkgConfig)

  # find HDF5
  find_package(HDF5 COMPONENTS CXX REQUIRED)
  include_directories(${HDF5_INCLUDE_DIRS})
  set(PROJECT_LIBS ${HDF5_LIBS} ${HDF5_LIBRARIES})

  # find HDF5pp
  pkg_check_modules(HDF5PP REQUIRED HDF5pp)
  include_directories(${HDF5PP_INCLUDE_DIRS})

  # create executable
  add_executable(${PROJECT_NAME} example.cpp)

  # link libraries
  target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})

