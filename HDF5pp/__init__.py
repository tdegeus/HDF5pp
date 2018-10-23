import warnings
warnings.filterwarnings("ignore")

import os, re, h5py

# ==================================================================================================

def abspath(path):
  r'''
Return absolute path.
  '''

  return os.path.normpath(os.path.join('/', path))

# ==================================================================================================

def join(*args, root=False):
  r'''
Join path components.
  '''

  if root: return os.path.join('/', *args)
  else   : return os.path.join(     *args)

# ==================================================================================================

def getdatasets(data, root='/'):
  r'''
Iterator to transverse all datasets across all groups in HDF5 file. Usage:

.. code-block:: python

  data = h5py.File('...', 'r')

  # loop over all paths
  for path in HDF5pp.getdatasets(data):
    print(path)

  # get a set of all datasets
  paths = set(HDF5pp.getdatasets(data))

  # get a list of all datasets
  paths = list(HDF5pp.getdatasets(data))

  data.close()

Read more in `this answer <https://stackoverflow.com/a/50720736/2646505>`_.

:arguments:

  **data** (``<h5py.File>``)
    A HDF5-archive.

:options:

  **root** ([``'/'``] | ``<str>``)
    Start a certain point along the path-tree.

:returns:

  Iterator.
  '''

  # ---------------------------------------------

  def iterator(g, prefix):

    for key in g.keys():

      item = g[key]

      path = join(prefix, key)

      if isinstance(item, h5py.Dataset):
        yield path

      elif isinstance(item, h5py.Group):
        yield from iterator(item, path)

  # ---------------------------------------------

  if isinstance(data[root], h5py.Dataset):

    yield root

  else:

    for path in iterator(data[root], root):
      yield path

# ==================================================================================================

def getpaths(data, root='/', max_depth=None, fold=None):
  r'''
Iterator to transverse all datasets across all groups in HDF5 file. Whereby one can choose to fold
(not transverse deeper than):

- Groups deeper than a certain ``max_depth``.
- A (list of) specific group(s).

:arguments:

  **data** (``<h5py.File>``)
    A HDF5-archive.

:options:

  **root** ([``'/'``] | ``<str>``)
    Start a certain point along the path-tree.

  **max_depth** (``<int>``)
    Set a maximum depth beyond which groups are folded.

  **fold** (``<list>``)
    Specify groups that are folded.

:returns:

  Iterator.

:example:

  Consider this file:

  .. code-block:: bash

    /path/to/first/a
    /path/to/first/b
    /data/c
    /data/d
    /e

  Calling:

  .. code-block:: python

    data = h5py.File('...', 'r')

    for path in HDF5pp.getpaths(data, max_depth=2, fold='/data'):
      print(path)

  Will print:

  .. code-block:: bash

    /path/to/...
    /data/...
    /e

  The ``...`` indicate it concerns a folded group, not a dataset. The first group was folded because
  of the maximum depth, and the second because if was specifically requested to be folded.
  '''

  if max_depth and fold:
    return _getpaths_fold_maxdepth(data, root, fold, max_depth)

  if max_depth:
    return _getpaths_maxdepth(data, root, max_depth)

  if fold:
    return _getpaths_fold(data, root, fold)

  return _getpaths(data, root)

# ==================================================================================================

def _getpaths(data, root):
  r'''
Specialization for ``getpaths``
  '''

  # ---------------------------------------------

  def iterator(g, prefix):

    for key in g.keys():

      item = g[key]

      path = join(prefix, key)

      if isinstance(item, h5py.Dataset):
        yield path

      elif isinstance(item, h5py.Group):
        yield from iterator(item, path)

  # ---------------------------------------------

  if isinstance(data[root], h5py.Dataset):

    yield root

  else:

    for path in iterator(data[root], root):
      yield path

# ==================================================================================================

def _getpaths_maxdepth(data, root, max_depth):
  r'''
Specialization for ``getpaths`` such that:

- Groups deeper than a certain maximum depth are folded.
  '''

  # ---------------------------------------------

  def iterator(g, prefix, max_depth):

    for key in g.keys():

      item = g[key]

      path = join(prefix, key)

      if isinstance(item, h5py.Dataset):
        yield path

      elif len(path.split('/'))-1 >= max_depth:
        yield path + '/...'

      elif isinstance(item, h5py.Group):
        yield from iterator(item, path, max_depth)

  # ---------------------------------------------

  if type(max_depth) == str: max_depth = int(max_depth)

  if isinstance(data[root], h5py.Dataset):

    yield root

  else:

    for path in iterator(data[root], root, max_depth):
      yield path

# ==================================================================================================

def _getpaths_fold(data, root, fold):
  r'''
Specialization for ``getpaths`` such that:

- Certain groups are folded.
  '''

  # ---------------------------------------------

  def iterator(g, prefix, fold):

    for key in g.keys():

      item = g[key]

      path = join(prefix, key)

      if isinstance(item, h5py.Dataset):
        yield path

      elif path in fold:
        yield path + '/...'

      elif isinstance(item, h5py.Group):
        yield from iterator(item, path, fold)

  # ---------------------------------------------

  if type(fold) == str: fold = [fold]

  if isinstance(data[root], h5py.Dataset):

    yield root

  else:

    for path in iterator(data[root], root, fold):
      yield path

# ==================================================================================================

def _getpaths_fold_maxdepth(data, root, fold, max_depth):
  r'''
Specialization for ``getpaths`` such that:

- Certain groups are folded.
- Groups deeper than a certain maximum depth are folded.
  '''

  # ---------------------------------------------

  def iterator(g, prefix, fold, max_depth):

    for key in g.keys():

      item = g[key]

      path = join(prefix, key)

      if isinstance(item, h5py.Dataset):
        yield path

      elif len(path.split('/'))-1 >= max_depth:
        yield path + '/...'

      elif path in fold:
        yield path + '/...'

      elif isinstance(item, h5py.Group):
        yield from iterator(item, path, fold, max_depth)

  # ---------------------------------------------

  if type(max_depth) == str: max_depth = int(max_depth)

  if type(fold) == str: fold = [fold]

  if isinstance(data[root], h5py.Dataset):

    yield root

  else:

    for path in iterator(data[root], root, fold, max_depth):
      yield path

# ==================================================================================================

def verify(data, datasets, error=False):
  r'''
Try reading each dataset of a list of datasets. Return a list with only those datasets that can be
successfully opened.
  '''

  # empty list of paths
  out = []

  # loop over list of paths
  for path in datasets:

    # - try reading, move to the next path if reading is unsuccessful
    try:
      tmp = data[path][...]
    except:
      if error: raise IOError('Error reading "{path:s}"'.format(path=path))
      else    : continue

    # - add to output
    out += [path]

  # return list of paths that can be successfully read
  return out

# ==================================================================================================

def exists(data, path):
  r'''
Check if a path exists in the HDF5-archive.
  '''

  if path in data:
    return True

  return False

# ==================================================================================================

def exists_any(data, paths):
  r'''
Check if any of the input paths exists in the HDF5-archive.
  '''

  if type(paths) == str: paths = [paths]

  for path in paths:
    if exists(data, path):
      return True

  return False

# ==================================================================================================

def exists_all(data, paths):
  r'''
Check if all of the input paths exists in the HDF5-archive.
  '''

  if type(paths) == str: paths = [paths]

  for path in paths:
    if not exists(data, path):
      return False

  return True

# ==================================================================================================

def copydatasets(source, dest, source_datasets, dest_datasets=None):
  r'''
Copy all datasets from one HDF5-archive 'source' to another HDF5-archive 'dest'. The datasets
can be renamed by specifying a list of 'dest_datasets' (whose entries should correspond to the
'source_datasets').
  '''

  # make sure that all paths are absolute paths
  source_datasets = [abspath(path) for path in source_datasets]

  # copy the source datasets
  if not dest_datasets: dest_datasets = [path for path in source_datasets]

  # check
  for dest_path in dest_datasets:
    if exists(dest, dest_path):
      raise IOError('Dateset "{0:s}" already exists'.format(dest_path))

  # get the group-names from 'source_datasets'
  # - read and filter duplicates
  groups = list(set([os.path.split(path)[0] for path in dest_datasets]))
  # - remove '/'
  groups = [group for group in groups if group != '/']
  # - sort based on depth
  groups = sorted(groups, key=lambda group: (group.count('/'), group))

  # create groups
  for group in groups:
    if not exists(dest, group):
      dest.create_group(group)

  # copy datasets
  for source_path, dest_path in zip(source_datasets, dest_datasets):

    # - get group name
    group = os.path.split(dest_path)[0]

    # - copy data
    source.copy(source_path, dest[group], os.path.split(dest_path)[1])
