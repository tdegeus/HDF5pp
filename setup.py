
import re

from setuptools import setup

# --------------------------------------------------------------------------------------------------

header = open('include/HDF5pp.h','r').read()
world  = re.split(r'(.*)(\#define HDF5PP_WORLD_VERSION\ )([0-9]+)(.*)',header)[3]
major  = re.split(r'(.*)(\#define HDF5PP_MAJOR_VERSION\ )([0-9]+)(.*)',header)[3]
minor  = re.split(r'(.*)(\#define HDF5PP_MINOR_VERSION\ )([0-9]+)(.*)',header)[3]

__version__ = '.'.join([world,major,minor])

# --------------------------------------------------------------------------------------------------

setup(
  name              = 'HDF5pp',
  version           = __version__,
  author            = 'Tom de Geus',
  author_email      = 'tom@geus.me',
  url               = 'https://github.com/tdegeus/HDF5pp',
  keywords          = 'HDF5, h5py',
  description       = 'Wrapper around h5py',
  long_description  = '',
  license           = 'MIT',
  packages          = ['HDF5pp'],
  scripts           = ['bin/HDF5pp_check', 'bin/HDF5pp_list', 'bin/HDF5pp_repair', 'bin/HDF5pp_find', 'bin/HDF5pp_merge', 'bin/HDF5pp_select'],
  install_requires  = ['docopt>=0.6.2', 'h5py>=2.8.0'],
      options={
        'build_scripts': {
            'executable': '/usr/bin/env python3',
        },
    },
)

# --------------------------------------------------------------------------------------------------
