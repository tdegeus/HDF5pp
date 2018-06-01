
from setuptools                 import setup
from setuptools.command.install import install

# --------------------------------------------------------------------------------------------------

__version__ = '0.0.1'

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
)

# --------------------------------------------------------------------------------------------------
