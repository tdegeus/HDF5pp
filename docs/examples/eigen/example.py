
import h5py
import numpy as np

f = h5py.File('example.h5','r')

print(f['mat_double'][:])
print(f['mat_size'  ][:])
print(f['vec_size'  ][:])
