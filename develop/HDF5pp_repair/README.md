
```bash
../../bin/HDF5pp_repair old.hdf5 new.hdf5 -f
```

Now 

```bash
../../bin/HDF5pp_list new.hdf5
```

should result in

```bash
/data
/test/data
/test/dir/data
/test/dir/data2
/tom/data
```
