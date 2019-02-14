# SS-CE

## Description

This is my attempt to organize all the C/C++ code I always use in a single place. This is meant to be used as a library, however without a particular purpose. Simply put: it's just a bunch of random C/C++ code which does nothing by itself.

## How to include in a CMake project

During installation a pkg-config file is also created. This should be usable by most build systems.

```CMake
#Example code for linking against libssce using CMake.
find_package( PkgConfig )
pkg_check_modules( SSCE REQUIRED ssce )
target_link_libraries( ${PROJECT_NAME} ${SSCE_CFLAGS} ${SSCE_LIBRARIES} )
```

## Building

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

- MATH
- STRINGS
- STRUCTURES

## Documentation

If [doxygen](www.doxygen.org) is installed then run `make doc` to generate the documentation. The docs are exported at doc_doxygen inside the build directory.

---

TODO: Under refactoring, jemalloc

---

### Clock _[MODULE\_CLOCK]_

Cross-platform high accuracy clock utilities. Usually implemented by calling the kernel.

```C
ssce_delay_hint();
ssce_delay(usecs);
ssce_msleep(msecs);
ssce_start(&pc);
ssce_stop(&pc);
ssce_reset(&pc);
```

### Threads _[MODULE\_THREADS]_

Pthreads but also work on Windows. Also adds a more 'C++' style header for C++ code.
Include one of the headers *Threads.h* or *Threads.hpp* to use this wrapper.

```C
//TODO
```

### Logger _[MODULE\_LOGGER]_

Basic logger. I have plans to expand this on the future.

```C
ssce_set_log_level(ALL);
ssce_logv("%s", "Hello");
ssce_logd("%s", "Hello");
ssce_logi("%s", "Hello");
ssce_logw("%s", "Hello");
ssce_loge("%s", "Hello");
ssce_logf("%s", "Hello");
ssce_checknull(p);
ssce_checkerrno(call_return);
```

### Command line parser _[MODULE\_CMDPARSER]_

Command line parser.

```C
ssce_parse_cmdln({...}, argc, argv);
```