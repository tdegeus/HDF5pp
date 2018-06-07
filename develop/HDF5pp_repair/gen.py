import h5py

data = h5py.File('old.hdf5','w')

data['/data'          ] = [1,2,3,4]
data['/tom/data'      ] = [1,2,3,4]
data['/test/data'     ] = [1,2,3,4]
data['/test/dir/data' ] = [1,2,3,4]
data['/test/dir/data2'] = [1,2,3,4]

data.close()
