
**************
LowFive in C++
**************

Teaser
======

.. code-block:: cpp

  #include <xtensor/xtensor.hpp>

  #include "LowFive.h"

  int main()
  {
    HighFive::File file("example.h5", HighFive::File::Overwrite);

    // write/read to string
    {
      std::string A = "test";

      LowFive::string::dump(file, "/path/to/string", A);

      std::string B = LowFive::string::cast(file, "/path/to/string");
    }

    // write/read to scalar
    {
      int A = 100;

      LowFive::scalar::dump(file, "/path/to/scalar", A);

      int B = LowFive::scalar::cast<int>(file, "/path/to/scalar");
    }

    // write/read to scalar to extendible dataset
    {
      int    A   = 100;
      size_t idx = 10;

      LowFive::scalar::dump(file, "/path/to/extendible", idx, A);

      int B = LowFive::scalar::cast<int>(file, "/path/to/extendible", idx);
    }

    // (over)write/read to xtensor
    {
      xt::xtensor<int,2> A = xt::ones<int>({10,5});

      LowFive::xtensor::dump(file, "/path/to/matrix", A);

      LowFive::xtensor::overwrite(file, "/path/to/matrix", A);

      xt::xtensor<double,2> B = LowFive::xtensor::cast<double,2>(file, "/path/to/matrix");
    }

    // check
    {
      if ( LowFive::exist(file, "/path/to/matrix") )
      {
        size_t size = LowFive::size(file, "/path/to/matrix");

        std::vector<size_t> shape = LowFive::shape(file, "/path/to/matrix");
      }
    }

    return 0;
  }
