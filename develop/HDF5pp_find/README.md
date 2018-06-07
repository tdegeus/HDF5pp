
# Delete

Delete file based on dataset existence:

```bash
../../bin/HDF5pp_find --not --iname data2 old.hdf5 --remove
```

should not delete `old.hdf5`, while

```bash
../../bin/HDF5pp_find --iname data2 old.hdf5 --remove
```

should delete it.

