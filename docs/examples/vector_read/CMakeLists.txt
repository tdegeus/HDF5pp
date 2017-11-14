cmake_minimum_required(VERSION 2.8.12)

# define a project name
project(example)

# set optimization level
# - compiler: ... -O3
set(CMAKE_BUILD_TYPE Release)

# set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# option switch on warnings, run : $ cmake .. -DWARNINGS=ON
option(WARNINGS "Show build warnings" OFF)
if(WARNINGS)
  if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
  else()
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  endif()
endif()

# load pkg-config
find_package(PkgConfig)

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
