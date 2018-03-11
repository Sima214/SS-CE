# SS-CE

## Description

This is my attempt to organize all the C/C++ code I always use in a single place. This is meant to be used as a library, however note that it doesn't have a clear purpose. Simply put: it's just a bunch of random C/C++ code which does nothing by itself.

## How to include in a CMake project

```CMake
find_package( PkgConfig )
pkg_check_modules( SSCE REQUIRED ssce )
```

And then use SSCE_INCLUDE_DIRS, SSCE_CFLAGS, SSCE_LIBRARIES.

## Building

This project uses cmake for generating makefiles for your system.

## Modules

This library is split into several modules which can be turned ON and OFF depending on the requirements of the application you are building.

### Clock

### Logger
