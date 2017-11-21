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

  File(){};

  File(std::string name, std::string mode="w", bool autoflush=true);

  // support functions
  // -----------------

  // flush all buffers associated with a file to disk
  void flush();

  // check if a path exists (is a group or a dataset)
  bool exists(std::string path);

  // create a group based on a specified path
  // NB "write" automatically does this, there is usually no need to call this function
  void createGroup(std::string path);

  // unlink a path
  // WARNING the space in the file may not be freed, use: $ h5repack file1 file2
  void unlink(std::string path);

  // read the shape of the data-matrix
  std::vector<size_t> shape(std::string path);

  // read the shape in a specific dimension of the data-matrix
  size_t shape(std::string path, size_t i);

  // read from file
  // --------------

  // read to specific data type, e.g. double, Eigen<double,...>, cppmat::matrix<double>, ...
  template<class T> T read(std::string path);

  // store scalar
  // ------------

  void write(std::string path, size_t data);
  void write(std::string path, float  data);
  void write(std::string path, double data);

  // store std::vector
  // -----------------

  void write(std::string path, const std::vector<float>  &input, const std::vector<size_t> &shape={});
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
};

// =================================================================================================
// constructor
// =================================================================================================

inline File::File(std::string name, std::string mode, bool autoflush)
{
  // open file
  if      ( mode == "w" ) m_fid = H5::H5File(name.c_str(),H5F_ACC_TRUNC );
  else if ( mode == "a" ) m_fid = H5::H5File(name.c_str(),H5F_ACC_RDWR  );
  else if ( mode == "r" ) m_fid = H5::H5File(name.c_str(),H5F_ACC_RDONLY);
  else    throw std::runtime_error("HDF5pp: unknown mode to open file");

  // store flush settings
  m_autoflush = autoflush;
}

// =================================================================================================
// support functions
// =================================================================================================

inline void File::flush()
{
  m_fid.flush(H5F_SCOPE_GLOBAL);
}

// -------------------------------------------------------------------------------------------------

inline bool File::exists(std::string path)
{
  // find first "/"
  size_t idx = path.find("/");

  // loop over all groups
  while ( true )
  {
    // - terminate if all "/" have been found
    if ( std::string::npos == idx )
      break;
    // - create group if needed
    if ( idx > 0 )
    {
      // -- get group name
      std::string name(path.substr(0,idx));
      // -- create if needed
      if ( !m_fid.exists(name.c_str()) ) return false;
    }
    // - proceed to next "/"
    idx = path.find("/",idx+1);
  }

  return m_fid.exists(path.c_str());
}

// -------------------------------------------------------------------------------------------------

inline void File::createGroup(std::string path)
{
  // find first "/"
  size_t idx = path.find("/");

  // loop over all groups
  while ( true )
  {
    // - terminate if all "/" have been found
    if ( std::string::npos == idx )
      return;
    // - create group if needed
    if ( idx > 0 )
    {
      // -- get group name
      std::string name(path.substr(0,idx));
      // -- create if needed
      if ( !m_fid.exists(name.c_str()) )
        H5::Group group = m_fid.createGroup(name.c_str());
    }
    // - proceed to next "/"
    idx = path.find("/",idx+1);
  }
}

// -------------------------------------------------------------------------------------------------

inline void File::unlink(std::string path)
{
  m_fid.unlink(path.c_str());
}

// =================================================================================================
// get shape of an array
// =================================================================================================

inline std::vector<size_t> File::shape(std::string path)
{
  // open dataset
  H5::DataSet   dataset   = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace = dataset.getSpace();

  // get the size in each direction
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - allocate as vector
  std::vector<size_t> shape(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - convert to vector
  for ( int i = 0 ; i < rank ; ++i ) shape[i] = static_cast<size_t>(dimsf[i]);

  // free memory
  free(dimsf);

  // return output
  return shape;
}

// -------------------------------------------------------------------------------------------------

inline size_t File::shape(std::string path, size_t i)
{
  // open dataset
  H5::DataSet   dataset   = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace = dataset.getSpace();

  // get the size in each direction
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - check rank
  if ( rank < static_cast<int>(i) )
    throw std::runtime_error("Cannot read, rank of data lower that requested");
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - get output
  size_t shape = static_cast<size_t>(dimsf[i]);

  // free memory
  free(dimsf);

  // return output
  return shape;
}

// =================================================================================================
// scalar
// =================================================================================================

inline void File::write(std::string path, size_t input)
{
  // create group(s) if needed
  createGroup(path);

  // pseudo-shape of the array
  hsize_t dimsf[1]; dimsf[0] = 1;

  // define storage shape / type
  H5::DataSpace dataspace(1,dimsf);
  H5::IntType   datatype(H5::PredType::NATIVE_HSIZE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(&input, H5::PredType::NATIVE_HSIZE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(std::string path, float input)
{
  // create group(s) if needed
  createGroup(path);

  // pseudo-shape of the array
  hsize_t dimsf[1]; dimsf[0] = 1;

  // define storage shape / type
  H5::DataSpace dataspace(1,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(&input, H5::PredType::NATIVE_FLOAT);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(std::string path, double input)
{
  // create group(s) if needed
  createGroup(path);

  // pseudo-shape of the array
  hsize_t dimsf[1]; dimsf[0] = 1;

  // define storage shape / type
  H5::DataSpace dataspace(1,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(&input, H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
inline size_t File::read<size_t>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();

  // check the size (cannot be >1)
  if ( rank != 0 )
  {
    // - allocate as HDF5-type
    hsize_t * dimsf = (hsize_t*)malloc(rank);
    // - read
    dataspace.getSimpleExtentDims(dimsf, NULL);
    // - total size
    size_t size = 0;
    for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);
    // - free memory
    free(dimsf);
    // - check size
    if ( size > 1 )
      throw std::runtime_error("Unable to read, data is array");
  }

  // allocate output
  size_t out;

  // read output
  dataset.read(&out, H5::PredType::NATIVE_HSIZE);

  // return output
  return out;
}

// -------------------------------------------------------------------------------------------------

template<>
inline double File::read<double>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();

  // check the size (cannot be >1)
  if ( rank != 0 )
  {
    // - allocate as HDF5-type
    hsize_t * dimsf = (hsize_t*)malloc(rank);
    // - read
    dataspace.getSimpleExtentDims(dimsf, NULL);
    // - total size
    size_t size = 0;
    for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);
    // - free memory
    free(dimsf);
    // - check size
    if ( size > 1 )
      throw std::runtime_error("Unable to read, data is array");
  }

  // allocate output
  double out;

  // read output
  dataset.read(&out, H5::PredType::NATIVE_DOUBLE);

  // return output
  return out;
}

// =================================================================================================
// std::vector
// =================================================================================================

inline void File::write(std::string path, const std::vector<float> &input, const std::vector<size_t> &shape)
{
  // create group(s) if needed
  createGroup(path);

  // copy input shape
  std::vector<size_t> dims = shape;

  // default shape == size of input
  if ( dims.size() == 0 )
  {
    dims.resize(1);
    dims[0] = input.size();
  }

  // shape of the array
  // - get the rank
  size_t rank = dims.size();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - copy
  for ( size_t i = 0 ; i < rank ; ++i )
    dimsf[i] = dims[i];

  // define storage shape / type
  H5::DataSpace dataspace(rank,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_FLOAT);

  // free memory
  free(dimsf);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(std::string path, const std::vector<double> &input, const std::vector<size_t> &shape)
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
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - copy
  for ( size_t i = 0 ; i < rank ; ++i )
    dimsf[i] = dims[i];

  // define storage shape / type
  H5::DataSpace dataspace(rank,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // free memory
  free(dimsf);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
inline std::vector<double> File::read<std::vector<double>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - total size
  size_t size = 0;
  for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);

  // allocate output
  std::vector<double> data(size);

  // read data
  dataset.read(const_cast<double*>(data.data()), H5::PredType::NATIVE_DOUBLE);

  // - free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
inline std::vector<size_t> File::read<std::vector<size_t>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - total size
  size_t size = 0;
  for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);

  // allocate output
  std::vector<size_t> data(size);

  // read data
  dataset.read(const_cast<size_t*>(data.data()), H5::PredType::NATIVE_HSIZE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// =================================================================================================
// Eigen matrix
// =================================================================================================

#ifdef HDF5PP_EIGEN

// -------------------------------------------------------------------------------------------------

inline void File::write(
  std::string path,
  const Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape == size (1-d)
  hsize_t dimsf[1];
  dimsf[0] = input.size();

  // define storage shape / type
  H5::DataSpace dataspace(1,dimsf);
  H5::IntType   datatype(H5::PredType::NATIVE_HSIZE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_HSIZE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape == size (1-d)
  hsize_t dimsf[1];
  dimsf[0] = input.size();

  // define storage shape / type
  H5::DataSpace dataspace(1,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(
  std::string path,
  const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array (rank == 2)
  hsize_t dimsf[2];
  dimsf[0] = input.rows();
  dimsf[1] = input.cols();

  // define storage shape / type
  H5::DataSpace dataspace(2,dimsf);
  H5::IntType   datatype(H5::PredType::NATIVE_HSIZE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_HSIZE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

inline void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array (rank == 2)
  hsize_t dimsf[2];
  dimsf[0] = input.rows();
  dimsf[1] = input.cols();

  // define storage shape / type
  H5::DataSpace dataspace(2,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
inline Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor>
File::read<Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - total size
  size_t size = 0;
  for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);

  // allocate output
  Eigen::Matrix<size_t, Eigen::Dynamic, 1, Eigen::ColMajor> data(size);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_HSIZE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
inline Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor>
File::read<Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - total size
  size_t size = 0;
  for ( int i = 0 ; i < rank ; ++i ) size += static_cast<size_t>(dimsf[i]);

  // allocate output
  Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> data(size);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
inline Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
File::read<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // check rank
  if ( rank != 2 )
    throw std::runtime_error("Unable to read, incorrect rank");

  // get the size in each direction
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);

  // allocate output
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data(dimsf[0],dimsf[1]);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
inline Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
File::read<Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // check rank
  if ( rank != 2 )
    throw std::runtime_error("Unable to read, incorrect rank");

  // get the size in each direction
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);

  // allocate output
  Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data(dimsf[0],dimsf[1]);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_HSIZE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

#endif

// =================================================================================================
// cppmat matrix
// =================================================================================================

#ifdef HDF5PP_CPPMAT

// -------------------------------------------------------------------------------------------------

inline void File::write(std::string path, const cppmat::matrix<double> &input)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  // - get the number of dimensions
  size_t rank = input.ndim();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - store shape in each direction
  for ( size_t i = 0 ; i < rank ; ++i )
    dimsf[i] = input.shape(i);

  // define storage shape / type
  H5::DataSpace dataspace(rank,dimsf);
  H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);

  // use little endian storage
  datatype.setOrder(H5T_ORDER_LE);

  // add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path.c_str(),datatype,dataspace);

  // store data
  dataset.write(input.data(), H5::PredType::NATIVE_DOUBLE);

  // free memory
  free(dimsf);

  // flush the file if so requested
  if ( m_autoflush ) flush();
}

// -------------------------------------------------------------------------------------------------

template<>
inline cppmat::matrix<double> File::read<cppmat::matrix<double>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_FLOAT )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::FloatType datatype = dataset.getFloatType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(double) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size in each direction
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - allocate
  std::vector<size_t> shape(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - convert to vector
  for ( int i = 0 ; i < rank ; ++i )
    shape[i] = static_cast<size_t>(dimsf[i]);

  // allocate output
  cppmat::matrix<double> data(shape);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  // - free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

template<>
inline cppmat::matrix<size_t> File::read<cppmat::matrix<size_t>>(std::string path)
{
  // open dataset
  H5::DataSet   dataset    = m_fid.openDataSet(path.c_str());
  H5::DataSpace dataspace  = dataset.getSpace();
  H5T_class_t   type_class = dataset.getTypeClass();

  // check data type
  if ( type_class != H5T_INTEGER )
    throw std::runtime_error("Unable to read, incorrect data-type");

  // check precision
  // - get storage type
  H5::IntType datatype = dataset.getIntType();
  // - get number of bytes
  size_t precision = datatype.getSize();
  // - check precision
  if ( precision != sizeof(size_t) )
    throw std::runtime_error("Unable to read, incorrect precision");

  // get the size in each direction
  // - read rank (a.k.a number of dimensions)
  int rank = dataspace.getSimpleExtentNdims();
  // - allocate as HDF5-type
  hsize_t * dimsf = (hsize_t*)malloc(rank);
  // - allocate
  std::vector<size_t> shape(rank);
  // - read
  dataspace.getSimpleExtentDims(dimsf, NULL);
  // - convert to vector
  for ( int i = 0 ; i < rank ; ++i )
    shape[i] = static_cast<size_t>(dimsf[i]);

  // allocate output
  cppmat::matrix<size_t> data(shape);

  // read data
  dataset.read(data.data(), H5::PredType::NATIVE_HSIZE);

  // free memory
  free(dimsf);

  // return output
  return data;
}

// -------------------------------------------------------------------------------------------------

#endif

// =================================================================================================

} // namespace H5p

// -------------------------------------------------------------------------------------------------

#endif
