
.. _tools:

******************
Command-line tools
******************

.. note::

  These tools use Python an depend on h5py as docopt. Get these tools for example using

  .. code-block:: bash

    pip3 install h5py doctopt

HDF5pp_repair
-------------

Copy as much as possible data from a corrupted HDF5 file.

HDF5pp_merge
------------

Merge a HDF5-file into another HDF5-file.

.. note::

  To merge a bunch of files one could use:

  .. code-block:: bash

    find . -iname '*.hdf5' -exec HDF5pp_merge {} output.hdf5 \;

  In this case ``HDF5pp_merge`` is called from each HDF5-file that is found. Note that if ``output.hdf5`` already existed, it is skipped by ``HDF5pp_merge``.
