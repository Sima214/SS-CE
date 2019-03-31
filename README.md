# SS-CE

[![linux status](https://img.shields.io/gitlab/pipeline/Sima214/SS-CE.svg?label=Linux)](https://gitlab.com/Sima214/SS-CE/commits/master)
[![osx status](https://img.shields.io/travis/com/Sima214/SS-CE.svg?label=MacOS)](https://travis-ci.com/Sima214/SS-CE)
[![windows status](https://img.shields.io/appveyor/ci/Sima214/SS-CE.svg?label=Windows)](https://ci.appveyor.com/project/Sima214/ss-ce)

## Description

This is my attempt to organize all the C/C++ code I always use in a single place. This is meant to be used as a library, however without a particular purpose. Simply put: it's just a bunch of random C/C++ code which does nothing by itself.

## How to include in a CMake project

During installation a pkg-config file is also created. This should be usable by most build systems.

```CMake
# Example code for linking against libssce using CMake.
find_package( PkgConfig )
pkg_check_modules( SSCE REQUIRED ssce )
target_compile_options( ${PROJECT_NAME} ${SSCE_CFLAGS} )
target_link_libraries( ${PROJECT_NAME} ${SSCE_LIBRARIES} )
```

## Building

Cmake, python3 and a valid gcc or clang installation is required!
This project uses cmake for generating makefiles for your system.
On linux this can simply be done by running:

```Shell
mkdir build
cd build
cmake ..
make -j4
sudo make install
```

## Modules

This library is split into several modules which can be turned ON and OFF depending on the requirements of the application you are building.

The list of available modules:

- CORE
- MATH
- CLOCK
- MEMORY
- STRINGS
- STRUCTURES
- LOGGER

## Documentation

If [doxygen](www.doxygen.org) is installed then run `make doc` to generate the documentation. The docs are generated in doc_doxygen inside the build directory.