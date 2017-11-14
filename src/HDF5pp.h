/* =================================================================================================

(c - MIT) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/HDF5pp

================================================================================================= */

#ifndef HDF5PP_H
#define HDF5PP_H

// -------------------------------------------------------------------------------------------------

// basic include
#include "H5Cpp.h"
#include <vector>

// optionally load Eigen
#ifdef EIGEN_WORLD_VERSION
#define HDF5PP_EIGEN
#endif

#ifdef HDF5PP_EIGEN
#include <Eigen/Eigen>
#endif

// optionally load cppmat
#ifdef CPPMAT_WORLD_VERSION
#define HDF5PP_CPPMAT
#endif

#ifdef HDF5PP_CPPMAT
#include <cppmat/cppmat.h>
#endif

// -------------------------------------------------------------------------------------------------

#define HDF5PP_WORLD_VERSION 0
#define HDF5PP_MAJOR_VERSION 0
#define HDF5PP_MINOR_VERSION 3

#define HDF5PP_VERSION_AT_LEAST(x,y,z) \
  (HDF5PP_WORLD_VERSION>x || (HDF5PP_WORLD_VERSION>=x && \
  (HDF5PP_MAJOR_VERSION>y || (HDF5PP_MAJOR_VERSION>=y && \
                              HDF5PP_MINOR_VERSION>=z))))

#define HDF5PP_VERSION(x,y,z) \
  (HDF5PP_WORLD_VERSION==x && \
   HDF5PP_MAJOR_VERSION==y && \
   HDF5PP_MINOR_VERSION==z)

// -------------------------------------------------------------------------------------------------

namespace H5p {

// =================================================================================================

class File
{
private:
  H5::H5File m_fid;
  bool       m_autoflush;

public:

  // constructor
  // -----------

  File(std::string name, std::string mode="w", bool autoflush=true);

  // support functions
  // -----------------

  // flush all buffers associated with a file to disk
  void flush();

  // create a group based on a specified path
  // NB "write" automatically does this, there is usually no need to call this function
  void createGroup(std::string path);

  // store scalar
  // ------------

  void write(std::string path, float data);

  void write(std::string path, double data);

  // store std::vector
  // -----------------

  void write(std::string path, const std::vector<float> &input, const std::vector<size_t> &shape={});

  void write(std::string path, const std::vector<double> &input, const std::vector<size_t> &shape={});

  // store Eigen array
  // -----------------

  #ifdef HDF5PP_EIGEN

  void write(
    std::string path,
    const Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor> &input
  );

  void write(
    std::string path,
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
  );

  void write(
    std::string path,
    const Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> &input
  );

  void write(
    std::string path,
    const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
  );

  #endif

  // store to / read from cppmat array
  // ---------------------------------

  #ifdef HDF5PP_CPPMAT

  void write(std::string path, const cppmat::matrix<double> &input);

  #endif

  // read from file
  // --------------

  template<class T>
  T read(std::string path);

  std::vector<size_t> shape(std::string path);

};

// =================================================================================================
// constructor
// =================================================================================================

File::File(std::string name, std::string mode, bool autoflush)
{
  // open file
  if      ( mode == "w" ) m_fid = H5::H5File(name,H5F_ACC_TRUNC );
  else if ( mode == "a" ) m_fid = H5::H5File(name,H5F_ACC_RDWR  );
  else if ( mode == "r" ) m_fid = H5::H5File(name,H5F_ACC_RDONLY);
  else    throw std::runtime_error("HDF5pp: unknown mode to open file");

  // store flush settings
  m_autoflush = autoflush;
}

// =================================================================================================
// support functions
// =================================================================================================

void File::flush()
{
  m_fid.flush(H5F_SCOPE_GLOBAL);
}

// -------------------------------------------------------------------------------------------------

void File::createGroup(std::string path)
{
  // suppress exceptions
  H5::Exception::dontPrint();

  // find first "/"
  size_t idx = path.find("/");

  // loop over all groups
  while ( true )
  {
    // - terminate if all "/" have been found
    if ( std::string::npos == idx )
      return;
    // - create group if needed
    if ( idx > 0 ) {
      std::string name(path.substr(0,idx));
      try         { H5::Group group = m_fid.openGroup  (name.c_str()); }
      catch (...) { H5::Group group = m_fid.createGroup(name.c_str()); }
    }
    // - proceed to next "/"
    idx = path.find("/",idx+1);
  }
}

// =================================================================================================
// std::vector
// =================================================================================================

void File::write(std::string path, const std::vector<float> &input, const std::vector<size_t> &shape)
{
  // create group(s) if needed
  createGroup(path);

  // copy input shape
  std::vector<size_t> dims = shape;

  // default shape
  if ( dims.size() == 0 )
  {
    dims.resize(1);
    dims[0] = input.size();
  }

  // shape of the array
  // - get the rank
  size_t rank = dims.size();
  // - allocate
  hsize_t dimsf[rank];
  // - copy
  for ( size_t i = 0 ; i < rank ; ++i )
    dimsf[i] = dims[i];

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(rank,dimsf);

  // define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_FLOAT);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

void File::write(std::string path, const std::vector<double> &input, const std::vector<size_t> &shape)
{
  // create group(s) if needed
  createGroup(path);

  // suppress exceptions
  H5::Exception::dontPrint();

  // copy input shape
  std::vector<size_t> dims = shape;

  // default shape
  if ( dims.size() == 0 )
  {
    dims.resize(1);
    dims[0] = input.size();
  }

  // shape of the array
  // - get the rank
  size_t rank = dims.size();
  // - allocate
  hsize_t dimsf[rank];
  // - copy
  for ( size_t i = 0 ; i < rank ; ++i )
    dimsf[i] = dims[i];

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(rank,dimsf);

  // define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  try
  {
    // - create data set
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);
  }
  catch( H5::FileIException error )
  {
    error.printError();
    throw std::runtime_error("HDF5pp: unable to write std::vector");
  }
  catch( H5::DataSetIException error )
  {
    error.printError();
    throw std::runtime_error("HDF5pp: unable to write std::vector");
  }
  catch( H5::DataSpaceIException error )
  {
    error.printError();
    throw std::runtime_error("HDF5pp: unable to write std::vector");
  }
  catch ( ... )
  {
    throw std::runtime_error("HDF5pp: unable to write std::vector");
  }

  // flush the file if so requested
  if ( m_autoflush ) flush();

}

// -------------------------------------------------------------------------------------------------

template<>
std::vector<double> File::read<std::vector<double>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);
  // - convert to vector (for cppmat)
  std::vector<size_t> dims(rank);
  for ( int i = 0 ; i < rank ; ++i )
    dims[i] = static_cast<size_t>(dims_out[i]);
  // - total size
  size_t size = 0;
  for ( auto &i : dims ) size += i;

  // allocate output
  std::vector<double> data(size);

  // read data
  dataset.read(const_cast<double*>(data.data()), H5::PredType::NATIVE_DOUBLE);

  // output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
std::vector<size_t> File::read<std::vector<size_t>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);
  // - convert to vector (for cppmat)
  std::vector<size_t> dims(rank);
  for ( int i = 0 ; i < rank ; ++i )
    dims[i] = static_cast<size_t>(dims_out[i]);
  // - total size
  size_t size = 0;
  for ( auto &i : dims ) size += i;

  // allocate output
  std::vector<size_t> data(size);

  // read data
  dataset.read(const_cast<size_t*>(data.data()), H5::PredType::NATIVE_HSIZE);

  // output
  return data;
}

// =================================================================================================
// Eigen matrix
// =================================================================================================

#ifdef HDF5PP_EIGEN

// -------------------------------------------------------------------------------------------------

void File::write(
  std::string path,
  const Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[1];
  dimsf[0] = input.size();

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(1,dimsf);

  // define storage type: float
  H5::IntType datatype(H5::PredType::NATIVE_HSIZE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_HSIZE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[1];
  dimsf[0] = input.size();

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(1,dimsf);

  // define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

void File::write(
  std::string path,
  const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[2];
  dimsf[0] = input.rows();
  dimsf[1] = input.cols();

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(2,dimsf);

  // define storage type: float
  H5::IntType datatype(H5::PredType::NATIVE_HSIZE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_HSIZE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[2];
  dimsf[0] = input.rows();
  dimsf[1] = input.cols();

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(2,dimsf);

  // define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
File::read<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);

  // allocate output
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data(dims_out[0],dims_out[1]);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  // output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
File::read<Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);

  // allocate output
  Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data(dims_out[0],dims_out[1]);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_HSIZE);

  // output
  return data;
}

// -------------------------------------------------------------------------------------------------

#endif

// =================================================================================================
// cppmat matrix
// =================================================================================================

#ifdef HDF5PP_CPPMAT

// -------------------------------------------------------------------------------------------------

void File::write(std::string path, const cppmat::matrix<double> &input)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  // - get the number of dimensions
  size_t nd = input.ndim();
  // - allocate depending on the dimensions of the input
  hsize_t dimsf[nd];
  // - store shape in each direction
  for ( size_t i = 0 ; i < nd ; ++i )
    dimsf[i] = input.shape(i);

  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(nd,dimsf);

  // define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
cppmat::matrix<double> File::read<cppmat::matrix<double>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);
  // - convert to vector (for cppmat)
  std::vector<size_t> dims(rank);
  for ( int i = 0 ; i < rank ; ++i )
    dims[i] = static_cast<size_t>(dims_out[i]);

  // allocate output
  cppmat::matrix<double> data(dims);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  // output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
cppmat::matrix<size_t> File::read<cppmat::matrix<size_t>>(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get type of the dataset
  H5T_class_t type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);
  // - convert to vector (for cppmat)
  std::vector<size_t> dims(rank);
  for ( int i = 0 ; i < rank ; ++i )
    dims[i] = static_cast<size_t>(dims_out[i]);

  // allocate output
  cppmat::matrix<size_t> data(dims);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_HSIZE);

  // output
  return data;
}

// -------------------------------------------------------------------------------------------------

#endif

// =================================================================================================
// scalar
// =================================================================================================

void File::write(std::string path, float input)
{
  std::vector<float> data(1);
  data[0] = static_cast<float>(input);
  write(path,data);
}

// -------------------------------------------------------------------------------------------------

void File::write(std::string path, double input)
{
  std::vector<double> data(1);
  data[0] = static_cast<double>(input);
  write(path,data);
}

// -------------------------------------------------------------------------------------------------

template<>
size_t File::read<size_t>(std::string path)
{
  std::vector<size_t> data = read<std::vector<size_t>>(path);
  return data[0];
}

// -------------------------------------------------------------------------------------------------

template<>
double File::read<double>(std::string path)
{
  std::vector<double> data = read<std::vector<double>>(path);
  return data[0];
}

// =================================================================================================
// get shape of an array
// =================================================================================================

std::vector<size_t> File::shape(std::string path)
{
  // open dataset
  H5::DataSet dataset = m_fid.openDataSet(path);

  // get the dataspace of the dataset
  H5::DataSpace dataspace = dataset.getSpace();

  // get the number of dimensions in the dataspace
  int rank = dataspace.getSimpleExtentNdims();

  // get the dimension size of each dimension in the dataspace
  // - allocate
  hsize_t dims_out[rank];
  // - read
  dataspace.getSimpleExtentDims(dims_out, NULL);
  // - convert to vector (for cppmat)
  std::vector<size_t> dims(rank);
  for ( int i = 0 ; i < rank ; ++i )
    dims[i] = static_cast<size_t>(dims_out[i]);

  // output
  return dims;
}

// =================================================================================================

} // namespace H5p

// -------------------------------------------------------------------------------------------------

#endif
