cmake_minimum_required(VERSION 3.16)

project(MiniFE
  LANGUAGES CXX
  VERSION 2.0
  DESCRIPTION "MiniFE: Mini Finite Element Application (Kokkos implementation)"
)

# --- OPTIONS --- #
option(MINIFE_ENABLE_MPI     "Build with MPI support"               ON)
option(MINIFE_USE_CSR_MATRIX "Use CSR matrix format (OFF = ELL)"    ON)
option(MINIFE_INFO           "Embed build info at compile time"      ON)
option(MINIFE_KERNELS        "Enable per-kernel timing"              OFF)

set(MINIFE_SCALAR         "double" CACHE STRING "Scalar type (double or float)")
set(MINIFE_LOCAL_ORDINAL  "int"    CACHE STRING "Local ordinal type")
set(MINIFE_GLOBAL_ORDINAL "int"    CACHE STRING "Global ordinal type (int or long long)")

# --- DEPENDENCIES --- #
find_package(Kokkos REQUIRED)
find_package(KokkosKernels REQUIRED)

if(MINIFE_ENABLE_MPI)
  find_package(MPI REQUIRED CXX)
endif()

# --- SOURCE FILES --- #
set(MINIFE_SOURCES
  src/main.cpp
  common/YAML_Doc.cpp
  common/YAML_Element.cpp
  utils/BoxPartition.cpp
  utils/mytimer.cpp
  utils/param_utils.cpp
  utils/utils.cpp
  kokkos/linalg/src/KokkosLinAlg_dummy.cpp
)

# --- EXECUTABLE --- #
add_executable(miniFE.x ${MINIFE_SOURCES})

# Include directories
target_include_directories(miniFE.x PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/fem
  ${CMAKE_CURRENT_SOURCE_DIR}/utils
  ${CMAKE_CURRENT_SOURCE_DIR}/common
  ${CMAKE_CURRENT_SOURCE_DIR}/kokkos/linalg/src
  ${CMAKE_CURRENT_SOURCE_DIR}/kokkos
)

# Kokkos
target_link_libraries(miniFE.x PRIVATE Kokkos::kokkos)
target_link_libraries(miniFE.x PRIVATE Kokkos::kokkoskernels)

# MPI
if(MINIFE_ENABLE_MPI)
  target_link_libraries(miniFE.x PRIVATE MPI::MPI_CXX)
  target_compile_definitions(miniFE.x PRIVATE HAVE_MPI MPICH_IGNORE_CXX_SEEK USE_MPI_WTIME)
endif()

# Matrix format
if(MINIFE_USE_CSR_MATRIX)
  target_compile_definitions(miniFE.x PRIVATE MINIFE_CSR_MATRIX)
else()
  target_compile_definitions(miniFE.x PRIVATE MINIFE_ELL_MATRIX)
endif()

# Scalar / ordinal types
target_compile_definitions(miniFE.x PRIVATE
  MINIFE_SCALAR=${MINIFE_SCALAR}
  MINIFE_LOCAL_ORDINAL=${MINIFE_LOCAL_ORDINAL}
  MINIFE_GLOBAL_ORDINAL=${MINIFE_GLOBAL_ORDINAL}
)

# Build info / kernel timing flags
if(MINIFE_INFO)
  target_compile_definitions(miniFE.x PRIVATE MINIFE_INFO=1)

  # Gather system info at configure time to produce miniFE_info.hpp
  cmake_host_system_information(RESULT MINIFE_HOSTNAME      QUERY HOSTNAME)
  cmake_host_system_information(RESULT MINIFE_KERNEL_NAME   QUERY OS_NAME)
  cmake_host_system_information(RESULT MINIFE_KERNEL_RELEASE QUERY OS_RELEASE)
  cmake_host_system_information(RESULT MINIFE_PROCESSOR     QUERY PROCESSOR_NAME)

  set(MINIFE_CXX "${CMAKE_CXX_COMPILER}")
  set(MINIFE_CXXFLAGS "${CMAKE_CXX_FLAGS}")

  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/miniFE_info.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/miniFE_info.hpp"
    @ONLY
  )
  target_include_directories(miniFE.x PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
else()
  target_compile_definitions(miniFE.x PRIVATE MINIFE_INFO=0)
endif()

if(MINIFE_KERNELS)
  target_compile_definitions(miniFE.x PRIVATE MINIFE_KERNELS=1)
else()
  target_compile_definitions(miniFE.x PRIVATE MINIFE_KERNELS=0)
endif()

target_compile_options(miniFE.x PRIVATE -O3 -fPIC)

# --- TESTING --- #
enable_testing()
add_test(NAME miniFE_basic
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/miniFE.x -nx 10 -ny 10 -nz 10
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
