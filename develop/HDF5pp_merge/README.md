
# Merge

The following

```bash
 ../../bin/HDF5pp_merge sim1/test.hdf5 new.hdf5 --dirname
 ../../bin/HDF5pp_merge sim2/test.hdf5 new.hdf5 --dirname
```

Now

```bash
../../bin/HDF5pp_list new.hdf5
```

should give

```
/sim1/data
/sim1/tom/data
/sim2/data
/sim2/tom/data
```
