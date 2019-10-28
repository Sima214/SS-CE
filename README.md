# SS-CE

[![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)

[![linux status](https://img.shields.io/gitlab/pipeline/Sima214/SS-CE.svg?label=Linux)](https://gitlab.com/Sima214/SS-CE/commits/master)
[![osx status](https://img.shields.io/travis/com/Sima214/SS-CE.svg?label=MacOS)](https://travis-ci.com/Sima214/SS-CE)
[![windows status](https://img.shields.io/appveyor/ci/Sima214/SS-CE.svg?label=Windows)](https://ci.appveyor.com/project/Sima214/ss-ce)

## Description

This is my attempt to organize all the C/C++ code I always use in a single place. Simply put: it's just a bunch of random C/C++ code bundled in a library.

## How to include in a CMake project

SS-CE can function as a cmake subproject.
To include add the following lines to the top level `CMakeLists.txt`:

```CMake
message( STATUS "Importing SS-CE..." )
# Here go any options for SS-CE.
add_subdirectory( "SS-CE" )
include_directories( ${SSCE_MODULE_INCLUDE} )
target_link_libraries( ${PROJECT_NAME} ${SSCE_MODULE_LIB} )
```

## Building

Cmake, python3 and a valid gcc or clang installation is required!
MSVC is not supported.

## Supported system configurations

### Operating systems

- Linux
- MacOS
- Windows

### Cpu Architectures

- amd64
- TODO: i386
- TODO: arm
- TODO: arm64

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
- TODO: AI

## Documentation

If [doxygen](www.doxygen.org) is installed then run `make doc` to generate the documentation. The docs are generated in doc_doxygen inside the build directory.
