/* =================================================================================================

(c - MIT) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/HDF5pp

================================================================================================= */

#ifndef LOWFIVE_H
#define LOWFIVE_H

#include <iostream>
#include <string>
#include <vector>

#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>

// optionally enable plug-in xtensor and load the library
#ifdef XTENSOR_VERSION_MAJOR
#define LOWFIVE_XTENSOR
#endif

#ifdef LOWFIVE_XTENSOR
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#endif

// =================================================================================================

namespace LowFive {

// -------------------------------------------------------------------------------------------------

inline bool exist(const HighFive::File &file, const std::string &path)
{
  // find first "/"
  size_t idx = path.find("/");

  // loop over all groups
  while ( true )
  {
    // - terminate if all "/" have been found
    if ( std::string::npos == idx ) break;
    // - create group if needed
    if ( idx > 0 )
    {
      // -- get group name
      std::string name(path.substr(0,idx));
      // -- create if needed
      if ( !file.exist(name) ) return false;
    }
    // - proceed to next "/"
    idx = path.find("/",idx+1);
  }

  return file.exist(path);
}

// -------------------------------------------------------------------------------------------------

inline void createGroup(HighFive::File &file, const std::string &path)
{
  // find first "/"
  size_t idx = path.find("/");

  // loop over all groups
  while ( true )
  {
    // terminate if all "/" have been found
    if ( std::string::npos == idx ) return;

    // create group if needed
    if ( idx > 0 )
    {
      // get group name
      std::string name(path.substr(0,idx));

      // create if needed
      if ( !file.exist(name) ) file.createGroup(name);
    }

    // proceed to next "/"
    idx = path.find("/",idx+1);
  }
}

// -------------------------------------------------------------------------------------------------

inline size_t size(const HighFive::File &file, const std::string &path)
{
  if ( ! exist(file,path) )
    throw std::runtime_error("LowFive::shape: Field does not exist ('"+path+"')");

  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  size_t size = 1;

  for ( size_t i = 0 ; i < dims.size() ; ++i ) size *= dims[i];

  return size;
}

// -------------------------------------------------------------------------------------------------

inline std::vector<size_t> shape(const HighFive::File &file, const std::string &path)
{
  if ( ! exist(file,path) )
    throw std::runtime_error("LowFive::shape: Field does not exist ('"+path+"')");

  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  std::vector<size_t> shape(dims.size());

  for ( size_t i = 0 ; i < dims.size() ; ++i ) shape[i] = dims[i];

  return shape;
}

// -------------------------------------------------------------------------------------------------

} // namespace ..

// =================================================================================================

namespace LowFive {
namespace scalar {

// -------------------------------------------------------------------------------------------------

template<class T>
inline HighFive::DataSet dump(HighFive::File &file, const std::string &path, T data)
{
  createGroup(file, path);

  HighFive::DataSet dataset = file.createDataSet<T>(path, HighFive::DataSpace::From(data));

  dataset.write(data);

  file.flush();

  return dataset;
}

// -------------------------------------------------------------------------------------------------

template<class T>
inline HighFive::DataSet dump(HighFive::File &file, const std::string &path, size_t idx, T data)
{
  createGroup(file, path);

  if ( exist(file, path) )
  {
    HighFive::DataSet dataset = file.getDataSet(path);

    dataset.select({idx}).write(data);

    return dataset;
  }

  size_t N = idx+1;

  HighFive::DataSpace dataspace = HighFive::DataSpace({N}, {HighFive::DataSpace::UNLIMITED});

  HighFive::DataSetCreateProps props;
  props.add(HighFive::Chunking(std::vector<hsize_t>{10}));

  HighFive::DataSet dataset = file.createDataSet(path, dataspace, HighFive::AtomicType<T>(), props);

  dataset.select({idx}).write(data);

  file.flush();

  return dataset;
}

// -------------------------------------------------------------------------------------------------

template<class T>
inline HighFive::DataSet overwrite(HighFive::File &file, const std::string &path, T data)
{
  if ( ! exist(file,path) ) return dump(file, path, data);

  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  if ( dims.size() != 0 )
    throw std::runtime_error("LowFive::scalar::overwrite: Field not a scalar ('"+path+"')");

  dataset.write(data);

  file.flush();

  return dataset;
}

// -------------------------------------------------------------------------------------------------

template <class T>
inline T cast(const HighFive::File &file, const std::string& path)
{
  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  if ( dims.size() != 0 )
    throw std::runtime_error("LowFive::scalar::cast: Field not a scalar ('"+path+"')");

  T data;

  dataset.read(data);

  return data;
}

// -------------------------------------------------------------------------------------------------

template <class T>
inline T cast(const HighFive::File &file, const std::string& path, size_t idx)
{
  HighFive::DataSet dataset = file.getDataSet(path);

  T data;

  dataset.select({idx}).read(data);

  return data;
}

// -------------------------------------------------------------------------------------------------

}} // namespace ...

// =================================================================================================

namespace LowFive {
namespace string {

// -------------------------------------------------------------------------------------------------

inline HighFive::DataSet dump(HighFive::File &file, const std::string &path, const std::string &data)
{
  createGroup(file, path);

  HighFive::DataSet dataset = file.createDataSet<std::string>(path, HighFive::DataSpace::From(data));

  dataset.write(data);

  file.flush();

  return dataset;
}

// -------------------------------------------------------------------------------------------------

inline std::string cast(const HighFive::File &file, const std::string& path)
{
  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  std::string data;

  dataset.read(data);

  return data;
}

// -------------------------------------------------------------------------------------------------

}} // namespace ...

// =================================================================================================

#ifdef LOWFIVE_XTENSOR

namespace LowFive {
namespace xtensor {

// -------------------------------------------------------------------------------------------------

template<class E>
inline HighFive::DataSet dump(HighFive::File &file, const std::string &path, const E &data)
{
  createGroup(file, path);

  std::vector<size_t> dims(data.shape().cbegin(), data.shape().cend());

  HighFive::DataSet dataset = file.createDataSet<typename E::value_type>(path, HighFive::DataSpace(dims));

  dataset.write(data.begin());

  file.flush();

  return dataset;
}

// -------------------------------------------------------------------------------------------------

template<class E>
inline HighFive::DataSet overwrite(HighFive::File &file, const std::string &path, const E &data)
{
  if ( ! exist(file,path) ) return dump(file, path, data);

  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  for ( size_t i = 0 ; i < data.shape().size() ; ++i )
    if ( data.shape()[i] != dims[i] )
      throw std::runtime_error("LowFive::xtensor::overwrite: Inconsistent dimensions ('"+path+"')");

  dataset.write(data.begin());

  return dataset;
}

// -------------------------------------------------------------------------------------------------

template<class T>
inline T cast_impl(const HighFive::File &file, const std::string& path)
{
  HighFive::DataSet dataset = file.getDataSet(path);

  auto dataspace = dataset.getSpace();

  auto dims = dataspace.getDimensions();

  T data = T::from_shape(dims);

  dataset.read(data.data());

  return data;
}

// -------------------------------------------------------------------------------------------------

template<class T>
auto cast(const HighFive::File &file, const std::string& path)
{
 return cast_impl<xt::xarray<T>>(file, path);
}

// -------------------------------------------------------------------------------------------------

template<class T, std::size_t dim>
auto cast(const HighFive::File &file, const std::string& path)
{
 return cast_impl<xt::xtensor<T, dim>>(file, path);
}

// -------------------------------------------------------------------------------------------------

}}  // namespace ...

#endif

// =================================================================================================

#endif
