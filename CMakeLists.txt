cmake_minimum_required(VERSION 3.9)
project(mpi)

# Add MPI Package to Project
find_package(MPI REQUIRED)


set(CMAKE_CXX_STANDARD 14)

add_executable(mpi mpi.cpp)
# Add libraries for code completion and compiling
target_link_libraries(mpi PUBLIC MPI::MPI_CXX)

add_executable(openmp openmp.c)
find_package(OpenMP REQUIRED)

target_link_libraries(openmp PRIVATE OpenMP::OpenMP_CXX)
