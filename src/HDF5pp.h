/* =================================================================================================

(c - MIT) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/HDF5pp

================================================================================================= */

#ifndef HDF5PP_H
#define HDF5PP_H

// -------------------------------------------------------------------------------------------------

#include "H5Cpp.h"
#include <vector>
#include <Eigen/Eigen>

// -------------------------------------------------------------------------------------------------

#define HDF5PP_WORLD_VERSION 0
#define HDF5PP_MAJOR_VERSION 0
#define HDF5PP_MINOR_VERSION 2

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
  File(std::string name, std::string mode="w", bool autoflush=true);

  // flush all buffers associated with a file to disk
  void flush();

  // store a double to an opened HDF5 file
  void write(
    std::string path,
    double      data,
    bool        asfloat=false
  );

  void write(
    std::string path,
    const std::vector<float> &input
  );

  void write(
    std::string path,
    const std::vector<double> &input,
    bool asfloat=false
  );

  // store a Eigen::Matrix to an opened HDF5 file
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
    const Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> &input,
    bool asfloat=false
  );

  void write(
    std::string path,
    const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input,
    bool asfloat=false
  );

  // create a group based on a specified path
  // NB "write" automatically does this, no need to call this function
  void createGroup(std::string path);
};

// =================================================================================================

File::File(std::string name, std::string mode, bool autoflush)
{
  m_fid       = H5::H5File(name,H5F_ACC_TRUNC);
  m_autoflush = autoflush;
}

// =================================================================================================

void File::flush()
{
  m_fid.flush( H5F_SCOPE_GLOBAL );
}

// =================================================================================================

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

void File::write(std::string path, double input, bool asfloat)
{
  if ( asfloat )
  {
    std::vector<float> data(1);
    data[0] = static_cast<float>(input);
    write(path,data);
  }
  else
  {
    std::vector<double> data(1);
    data[0] = input;
    write(path,data);
  }

  if ( m_autoflush ) flush();
}

// =================================================================================================

void File::write(std::string path, const std::vector<float> &input)
{
// create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[1];
  dimsf[0] = input.size();
  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(1,dimsf);

  // - define storage type: float
  H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
  // - define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);
  // - add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
  // - store data
  dataset.write(input.data(),H5::PredType::NATIVE_FLOAT);

  if ( m_autoflush ) flush();
}

// =================================================================================================

void File::write(std::string path, const std::vector<double> &input, bool asfloat)
{
// create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[1];
  dimsf[0] = input.size();
  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(1,dimsf);

  if ( asfloat )
  {
    // - store matrix as contiguous array
    std::vector<float> data(input.size());
    for ( size_t i = 0 ; i < input.size() ; ++i )
      data[i] = static_cast<float>(input[i]);
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(data.data(),H5::PredType::NATIVE_FLOAT);
  }
  else
  {
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(input.data(),H5::PredType::NATIVE_DOUBLE);
  }

  if ( m_autoflush ) flush();
}

// =================================================================================================

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

  // - define storage type: float
  H5::IntType datatype(H5::PredType::NATIVE_HSIZE);
  // - define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);
  // - add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
  // - store data
  dataset.write(input.data(),H5::PredType::NATIVE_HSIZE);

  if ( m_autoflush ) flush();
}

// =================================================================================================

void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> &input,
  bool asfloat
)
{
  // create group(s) if needed
  createGroup(path);

  // shape of the array
  hsize_t dimsf[1];
  dimsf[0] = input.size();
  // convert shape -> H5::DataSpace
  H5::DataSpace dataspace(1,dimsf);

  // store as floats (not as doubles)
  if ( asfloat )
  {
    // - store matrix as contiguous array
    std::vector<float> data(input.size());
    for ( size_t i = 0 ; i < static_cast<size_t>(input.size()) ; ++i )
      data[i] = static_cast<float>(input(i));
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(&data[0],H5::PredType::NATIVE_FLOAT);
  }
  // store as doubles
  else
  {
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(input.data(),H5::PredType::NATIVE_DOUBLE);
  }

  if ( m_autoflush ) flush();
}

// =================================================================================================

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

  // - define storage type: float
  H5::IntType datatype(H5::PredType::NATIVE_HSIZE);
  // - define storage type: little endian
  datatype.setOrder(H5T_ORDER_LE);
  // - add data-set to file
  H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
  // - store data
  dataset.write(input.data(),H5::PredType::NATIVE_HSIZE);

  if ( m_autoflush ) flush();
}

// =================================================================================================

void File::write(
  std::string path,
  const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &input,
  bool asfloat
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

  // store as floats (not as doubles)
  if ( asfloat )
  {
    // - store matrix as contiguous array
    std::vector<float> data(input.size());
    for ( size_t i = 0 ; i < static_cast<size_t>(input.size()) ; ++i )
      data[i] = static_cast<float>(input(i));
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(&data[0],H5::PredType::NATIVE_FLOAT);
  }
  // store as doubles
  else
  {
    // - define storage type: float
    H5::FloatType datatype(H5::PredType::NATIVE_DOUBLE);
    // - define storage type: little endian
    datatype.setOrder(H5T_ORDER_LE);
    // - add data-set to file
    H5::DataSet dataset = m_fid.createDataSet(path,datatype,dataspace);
    // - store data
    dataset.write(input.data(),H5::PredType::NATIVE_DOUBLE);
  }

  if ( m_autoflush ) flush();
}

// =================================================================================================

} // namespace H5p

// -------------------------------------------------------------------------------------------------

#endif
