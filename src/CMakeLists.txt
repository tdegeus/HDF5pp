
# required to specify the c++ standard
cmake_minimum_required(VERSION 3.0)

# define project information
project(HDF5pp)

# automatically parse the version number
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/HDF5pp.h" _HDF5pp_version_header)
string(REGEX MATCH "define[ \t]+HDF5PP_WORLD_VERSION[ \t]+([0-9]+)" _HDF5pp_world_version_match "${_HDF5pp_version_header}")
set(HDF5PP_WORLD_VERSION "${CMAKE_MATCH_1}")
string(REGEX MATCH "define[ \t]+HDF5PP_MAJOR_VERSION[ \t]+([0-9]+)" _HDF5pp_major_version_match "${_HDF5pp_version_header}")
set(HDF5PP_MAJOR_VERSION "${CMAKE_MATCH_1}")
string(REGEX MATCH "define[ \t]+HDF5PP_MINOR_VERSION[ \t]+([0-9]+)" _HDF5pp_minor_version_match "${_HDF5pp_version_header}")
set(HDF5PP_MINOR_VERSION "${CMAKE_MATCH_1}")
set(HDF5PP_VERSION_NUMBER ${HDF5PP_WORLD_VERSION}.${HDF5PP_MAJOR_VERSION}.${HDF5PP_MINOR_VERSION})

# required for install
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# set c++ standard -> c++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 14)
  set(CMAKE_CXX_STANDARD 14)
endif()

# set paths where 'HDF5pp' will be installed
# - root
set(HDF5PP_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
# - headers
set(HDF5PP_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}")
# - headers
set(INCLUDE_INSTALL_DIR
  "${CMAKE_INSTALL_INCLUDEDIR}/HDF5pp"
  CACHE PATH "Path, relative to CMAKE_PREFIX_PATH, where the HDF5pp header files are installed"
)
# - CMake configuration
set(CMAKEPACKAGE_INSTALL_DIR
  "${CMAKE_INSTALL_DATADIR}/HDF5pp"
  CACHE PATH "Path where HDF5ppConfig.cmake is installed"
)
# - pkg-config
set(PKGCONFIG_INSTALL_DIR
  "${CMAKE_INSTALL_DATADIR}/pkgconfig"
  CACHE PATH "Path where HDF5pp.pc is installed"
)

# list all header files
set(HDF5PP_HEADERS
  HDF5pp.h
)

# Disable pkg-config for native Windows builds
if(NOT WIN32 OR NOT CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
  option(HDF5PP_BUILD_PKGCONFIG "Build pkg-config .pc file for HDF5pp" ON)
endif()

# build pkg-config file -> fill in relevant values
if(HDF5PP_BUILD_PKGCONFIG)
  configure_file(HDF5pp.pc.in HDF5pp.pc @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/HDF5pp.pc DESTINATION ${PKGCONFIG_INSTALL_DIR})
endif()

# configure CMake file
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/HDF5ppConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/HDF5ppConfig.cmake
  PATH_VARS HDF5PP_INCLUDE_DIR HDF5PP_ROOT_DIR
  INSTALL_DESTINATION ${CMAKEPACKAGE_INSTALL_DIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install
# - CMake-file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/HDF5ppConfig.cmake
  DESTINATION
  ${CMAKEPACKAGE_INSTALL_DIR}
)
# - headers
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/${HDF5PP_HEADERS}
  DESTINATION
  ${INCLUDE_INSTALL_DIR}
)

# print information to screen
message(STATUS "")
message(STATUS "+---------------------------------------------------------------------------------")
message(STATUS "|")
message(STATUS "| Use 'make install' to install in '${CMAKE_INSTALL_PREFIX}'")
message(STATUS "| To specify a custom directory call")
message(STATUS "|   cmake . -DCMAKE_INSTALL_PREFIX=yourprefix")
message(STATUS "| For custom paths, add the following line to your '~/.bashrc'")
message(STATUS "|   export PKG_CONFIG_PATH=${CMAKE_INSTALL_PREFIX}/share/pkgconfig:$PKG_CONFIG_PATH")
message(STATUS "|")
message(STATUS "+---------------------------------------------------------------------------------")
message(STATUS "")
