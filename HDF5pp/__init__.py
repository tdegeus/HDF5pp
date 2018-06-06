
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

def getdatasets(data, root='/'):
  r'''
Traverse all datasets across all groups in HDF5 file. Usage:

*   for i in HDF5pp.getdatasets(data): ...
*   set(HDF5pp.getdatasets(data))
*   list(HDF5pp.getdatasets(data))
  '''

  def iterator(g, prefix=''):

    for key in g.keys():

      item = g[key]

      path = '{}/{}'.format(prefix, key)

      if isinstance(item, h5py.Dataset): # test for dataset
        yield (path, item)

      elif isinstance(item, h5py.Group): # test for group (go down)
        yield from iterator(item, path)

  if root == '/':
    prefix = ''

  else:
    prefix = root
    if isinstance(data[root], h5py.Dataset):
      yield root
      return

  for (path, dset) in iterator(data[root], prefix):
    yield path

# ==================================================================================================

def verify(data, datasets):
  r'''
Try reading each dataset of a list of datasets. Return a list with only those datasets that can be
successfully opened.
  '''

  # empty list of paths
  out = []

  # loop over list of paths
  for path in datasets:

    # - try reading, move to the next path if reading is unsuccessful
    try   : data[path]
    except: continue

    # - add to output
    out += [path]

  # return list of paths that can be successfully read
  return out

# ==================================================================================================

def exists(data, datasets):
  r'''
Return the first path that exists in the HDF5-archive.
  '''

  if type(datasets) == str: datasets = [datasets]

  for path in datasets:
    if path in data:
      return path

  return False

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

  # get the group-names from 'source_datasets'
  # - read and filter duplicates
  groups = list(set([os.path.split(path)[0] for path in dest_datasets]))
  # - remove '/'
  groups = [group for group in groups if group != '/']
  # - sort based on depth
  groups = sorted(groups, key=lambda group: (group.count('/'), group))

  # create groups
  for group in groups: dest.create_group(group)

  # copy datasets
  for source_path, dest_path in zip(source_datasets, dest_datasets):

    # - get group name
    group = os.path.split(dest_path)[0]

    # - copy data
    source.copy(source_path, dest[group], os.path.split(dest_path)[1])
