import h5py

data = h5py.File('sim1/test.hdf5','w')

data['/data'      ] = [1,2,3,4]
data['/tom/data'  ] = [1,2,3,4]

data.close()

data = h5py.File('sim2/test.hdf5','w')

data['/data'      ] = [1,2,3,4]
data['/tom/data'  ] = [1,2,3,4]

data.close()
