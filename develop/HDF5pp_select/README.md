
For this file:

```bash
../../bin/HDF5pp_list old.hdf5
```

with content

```bash
/data
/test/data
/test/dir/data
/test/dir/data2
/tom/data
```

The command

```
../../bin/HDF5pp_select -j select.json old.hdf5
```

should prompt 

```bash
The following paths are not copied:
  /tom/data
Continue [y/n]? y
```

Then 

```bash
../../bin/HDF5pp_list new.hdf5
```
should give 

```bash
/data
/experiment/data
/experiment/dir/data
/experiment/dir/data2
```
