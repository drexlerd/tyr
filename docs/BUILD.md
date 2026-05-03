# Building Tyr (C++)

Before building the project, ensure you have the following installed:

1. **CMake** (version 3.21 or higher)
2. **Python >=3.9** with **Development Headers** and the **typing_extensions** module for stub generation of the Python bindings.
3. **C++20 Compiler**

If no C++20 compliant compiler is easily available, we provide install instructions for GCC 14.2.0 (Linux) from source at the bottom of the page.

## Installing the Dependencies

Tyr depends on the following set of libraries:

- [Boost](https://www.boost.org/) header-only libraries (Fusion, Spirit x3, Container, BGL),
- [Cista](https://github.com/felixguendling/cista/) for zero-copy serialization,
- [GoogleBenchmark](https://github.com/google/benchmark) for automated performance benchmarking,
- [GoogleTest](https://github.com/google/googletest) for unit testing,
- [fmt](https://github.com/fmtlib/fmt) for formatting,
- [gtl](https://github.com/greg7mdp/gtl) for flat hash sets,
- [oneTBB](https://github.com/uxlfoundation/oneTBB) for multi core utilization,
- [argparse](https://github.com/p-ranav/argparse.git) for argument parsing, and 
- [nanobind](https://github.com/wjakob/nanobind.git) for Python bindings.


Dependencies that are owned by Loki, such as Boost, fmt, argparse, and gtl, are installed through Loki's dependency superbuild and are reused by Tyr through the same install prefix.

Keep `dependencies/` source-only. Do not place dependency build or install artifacts inside it; use `dependencies-build/` and `dependencies-install/` instead.

Choose one of the following complete recipes.

### Static Dependency Build

This is the default standalone C++ build. Dependencies are built and consumed statically where supported.

```console
cmake -S dependencies -B dependencies-build \
  -DCMAKE_INSTALL_PREFIX=${PWD}/dependencies-install \
  -DCMAKE_PREFIX_PATH=${PWD}/dependencies-install

cmake --build dependencies-build -j$(nproc)

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=${PWD}/dependencies-install

cmake --build build -j$(nproc)
```

### Shared Dependency Build

Use this for Python wheels, downstream Python/C++ packages, and local builds where multiple extension modules should link to the same native libraries. The dependency build and the Tyr configure step must agree: shared dependencies require `TYR_LINK_STATIC_DEPENDENCIES=OFF`.

```console
cmake -S dependencies -B dependencies-build \
  -DCMAKE_INSTALL_PREFIX=${PWD}/dependencies-install \
  -DCMAKE_PREFIX_PATH=${PWD}/dependencies-install \
  -DTYR_BUILD_SHARED_DEPENDENCIES=ON \
  -DCMAKE_INSTALL_LIBDIR=lib

cmake --build dependencies-build -j$(nproc)

cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=${PWD}/dependencies-install \
  -DTYR_BUILD_SHARED=ON \
  -DTYR_LINK_STATIC_DEPENDENCIES=OFF

cmake --build build -j$(nproc)
```

## Building Tyr

The recipes above build Tyr in `build/`. To install Tyr from that build directory to a chosen installation prefix:

```console
cmake --install build --prefix=<path/to/installation-directory>
```

The compiled executables should now be in either the build directory or the installation directory.


# Installing a C++20 Compiler From Source (Optional)

Run the following sequence of commands to download, configure, build, and install GCC from source.

1. Installing GMP

```console
wget https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.2.1.tar.bz2
tar -xjf gmp-6.2.1.tar.bz2
mv gmp-6.2.1 gmp-6.2.1-src
export GMP_INSTALL_DIR=<path/to_installation>
cd gmp-6.2.1-src
./configure --prefix=$GMP_INSTALL_DIR
make -j$(nproc)
make install
cd ..
```

2. Installing MPFR

```console
wget https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-4.1.0.tar.bz2
tar -xjf mpfr-4.1.0.tar.bz2
mv mpfr-4.1.0 mpfr-4.1.0-src
export MPFR_INSTALL_DIR=<path/to_installation>
cd mpfr-4.1.0-src
./configure --prefix=$MPFR_INSTALL_DIR --with-gmp=$GMP_INSTALL_DIR
make -j$(nproc)
make install
cd ..
```

3. Installing MPC

```console
wget https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.2.1.tar.gz
tar -xzf mpc-1.2.1.tar.gz
mv mpc-1.2.1 mpc-1.2.1-src
export MPC_INSTALL_DIR=<path/to_installation>
cd mpc-1.2.1-src
./configure --prefix=$MPC_INSTALL_DIR --with-gmp=$GMP_INSTALL_DIR --with-mpfr=$MPFR_INSTALL_DIR
make -j$(nproc)
make install
cd ..
```

4. Installing GCC

```console
git clone git@github.com:gcc-mirror/gcc.git gcc-14.2.0
cd gcc-14.2.0
git checkout releases/gcc-14.2.0
export GCC_INSTALL_DIR=<path/to_installation>
./configure --prefix=$GCC_INSTALL_DIR --enable-languages=c,c++ --disable-multilib --with-gmp=$GMP_INSTALL_DIR --with-mpfr=$MPFR_INSTALL_DIR --with-mpc=$MPC_INSTALL_DIR
make -j$(nproc)
make install
```

5. Update environment variables

```console
export CC=${GCC_INSTALL_DIR}/bin/gcc
export CXX=${GCC_INSTALL_DIR}/bin/g++
export PATH=${GCC_INSTALL_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${GCC_INSTALL_DIR}/lib64:$LD_LIBRARY_PATH
export LIBRARY_PATH=${GCC_INSTALL_DIR}/lib64:$LIBRARY_PATH
export CPATH=${GCC_INSTALL_DIR}/include:$CPATH
```
