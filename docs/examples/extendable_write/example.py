
import h5py
import numpy as np

f = h5py.File('example.hdf5','r')

print(f['/data'][:])
