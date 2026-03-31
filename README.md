# kokkos-comm-miniFE

This is a modified version of mantevo/miniFE from SHA abe328816d84afc319c482d6bc8df414b8f90d79 with Kokkos Comm support

## Getting Started

### Building the Kokkos implementation

Kokkos and Kokkos Kernels must be installed before building miniFE.

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

**3. Configure and build MiniFE**

```bash
cmake -S . -B build \
  -DKokkos_ROOT=$PWD/_deps/kokkos-install \
  -DKokkosKernels_ROOT=$PWD/_deps/kernels-install \
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

**3. Run**

```bash
./kokkos-build-minife/miniFE.x -nx 30 -ny 30 -nz 30
```

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
