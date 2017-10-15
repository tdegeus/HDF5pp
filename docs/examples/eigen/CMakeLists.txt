cmake_minimum_required(VERSION 2.8.12)

# define a project name
project(example)

# set optimization level
# - compiler: ... -O3
set(CMAKE_BUILD_TYPE Release)

# load pkg-config
find_package(PkgConfig)

# find eigen3
# - compiler: ... -I${EIGEN3_INCLUDE_DIRS}
pkg_check_modules(EIGEN3 REQUIRED eigen3)
include_directories(${EIGEN3_INCLUDE_DIRS})

# find HDF5
find_package(HDF5 COMPONENTS CXX REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
set(HDF5_LIBS ${HDF5_LIBS} ${HDF5_LIBRARIES})

# find HDF5pp
# - compiler: ... -I${HDF5PP_INCLUDE_DIRS}
pkg_check_modules(HDF5PP REQUIRED HDF5pp)
include_directories(${HDF5PP_INCLUDE_DIRS})

# add executable
# - compiler: ${CXX} -o ${PROJECT_NAME} example.cpp ...
add_executable(${PROJECT_NAME} example.cpp)

# link libraries
# - compiler: ... -lname1 -lname2
target_link_libraries(${PROJECT_NAME} ${HDF5_LIBS})
