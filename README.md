# kokkos-comm-miniFE

This is a modified version of mantevo/miniFE from SHA abe328816d84afc319c482d6bc8df414b8f90d79 with Kokkos Comm support

## Getting Started

### Building the Kokkos implementation

Kokkos, Kokkos Kernels, and Kokkos Comm must be installed before building miniFE.

**1. Install Kokkos**

```bash
git clone --depth=1 https://github.com/kokkos/kokkos.git _deps/kokkos-src
cmake -S _deps/kokkos-src -B _deps/kokkos-build \
  -DCMAKE_INSTALL_PREFIX=$PWD/_deps/kokkos-install \
  -DKokkos_ENABLE_SERIAL=ON \
  -DKokkos_ENABLE_OPENMP=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build _deps/kokkos-build --parallel 2
cmake --install _deps/kokkos-build
```

**2. Install Kokkos Kernels**
```bash
git clone --depth=1 https://github.com/kokkos/kokkos-kernels.git _deps/kernels-src
cmake -S _deps/kernels-src -B _deps/kernels-build \
  -DKokkos_ROOT=$PWD/_deps/kokkos-install \
  -DCMAKE_INSTALL_PREFIX=$PWD/_deps/kernels-install \
  -DKokkosKernels_ENABLE_ALL_COMPONENTS=OFF \
  -DKokkosKernels_ENABLE_COMPONENT_SPARSE=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build _deps/kernels-build --parallel 2
cmake --install _deps/kernels-build
```

**3. Install Kokkos Comm**
```bash
git clone --depth=1 https://github.com/kokkos/kokkos-comm.git _deps/comm-src
cmake -S _deps/comm-src -B _deps/comm-build \
  -DKokkos_ROOT=$PWD/_deps/kokkos-install \
  -DCMAKE_INSTALL_PREFIX=$PWD/_deps/comm-install \
  -DCMAKE_BUILD_TYPE=Release
cmake --build _deps/comm-build --parallel 2
cmake --install _deps/comm-build
```

**4. Configure and build MiniFE**
```bash
cmake -S . -B build \
  -DKokkos_ROOT=$PWD/_deps/kokkos-install \
  -DKokkosKernels_ROOT=$PWD/_deps/kernels-install \
  -DKokkosComm_ROOT=$PWD/_deps/comm-install \
  -DCMAKE_CXX_COMPILER=mpicxx \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Key CMake options:

| Option | Default | Description |
|---|---|---|
| `MINIFE_ENABLE_MPI` | `ON` | Build with MPI support |
| `MINIFE_USE_CSR_MATRIX` | `ON` | Use CSR format (`OFF` for ELL) |
| `MINIFE_SCALAR` | `double` | Scalar type (`double` or `float`) |
| `MINIFE_LOCAL_ORDINAL` | `int` | Local ordinal type |
| `MINIFE_GLOBAL_ORDINAL` | `int` | Global ordinal type (`int` or `long long`) |
| `MINIFE_INFO` | `ON` | Embed build info |
| `MINIFE_KERNELS` | `OFF` | Enable per-kernel timing |
| `MINIFE_DEBUG` | `OFF` | Write matrix/vector files to disk (required for gold file comparison) |

**3. Run**

```bash
# Single process
./build/miniFE.x -nx 30 -ny 30 -nz 30

# Multiple MPI ranks
mpirun -np 4 ./build/miniFE.x -nx 30 -ny 30 -nz 30
```

This produces a YAML results file named `miniFE.<nx>x<ny>x<nz>.P<numprocs>.<timestamp>.yaml`.

To verify the numerical solution against an analytic reference, pass `verify_solution=1`:

```bash
./build/miniFE.x -nx 10 -ny 10 -nz 10 verify_solution=1
```

### Comparing against gold files

The `gold_files/` directory contains reference matrix and vector snapshots for a small `nx=1 ny=1 nz=2` problem run with 1 and 2 MPI ranks. File names follow the pattern `1x1x2_<name>.<numprocs>.<rank>`.

**1. Build with debug output enabled**

```bash
cmake -S . -B build-debug \
  -DKokkos_ROOT=$PWD/_deps/kokkos-install \
  -DKokkosKernels_ROOT=$PWD/_deps/kernels-install \
  -DCMAKE_CXX_COMPILER=mpicxx \
  -DCMAKE_BUILD_TYPE=Release \
  -DMINIFE_DEBUG=ON
cmake --build build-debug
```

**2. Run and compare (1 MPI rank)**

```bash
cd /tmp
mpirun -np 1 /path/to/build-debug/miniFE.x -nx 1 -ny 1 -nz 2

diff A.mtx.1.0   /path/to/gold_files/1x1x2_A.mtx.1.0
diff b.vec.1.0   /path/to/gold_files/1x1x2_b.vec.1.0
diff x.vec.1.0   /path/to/gold_files/1x1x2_x.vec.1.0
```

**3. Run and compare (2 MPI ranks)**

```bash
mpirun -np 2 /path/to/build-debug/miniFE.x -nx 1 -ny 1 -nz 2

diff A.mtx.2.0   /path/to/gold_files/1x1x2_A.mtx.2.0
diff A.mtx.2.1   /path/to/gold_files/1x1x2_A.mtx.2.1
diff b.vec.2.0   /path/to/gold_files/1x1x2_b.vec.2.0
diff b.vec.2.1   /path/to/gold_files/1x1x2_b.vec.2.1
diff x.vec.2.0   /path/to/gold_files/1x1x2_x.vec.2.0
diff x.vec.2.1   /path/to/gold_files/1x1x2_x.vec.2.1
```

The `MINIFE_DEBUG` build writes `A.mtx`, `b.vec`, and `x.vec` (post boundary-condition, post localization, and solution respectively), each suffixed with `.<numprocs>.<rank>` matching the gold file naming convention.

## Modifications

- [x] Make -> CMake build system
- [x] StaticCrsGraph moved into Kokkos Kernels
- [ ] Remove all references to KOKKOS_USE_CUSPARSE
- [ ] Remove all references to KOKKOS_USE_MKL
- [x] Bring Kokkos_Vector.hpp in-tree
- [x] Rework deprecated DualView.d_view/h_view
- [x] View::Rank -> rank
- [x] Fix dual-view sync behavior for device code

## License

MiniFE is available under LGPL-3
