cmake_minimum_required(VERSION 3.23)
project(mpi)

# Add MPI Package to Project
find_package(MPI REQUIRED)


set(CMAKE_CXX_STANDARD 14)

add_executable(mpi main.cpp)
# Add libraries for code completion and compiling
target_link_libraries(mpi PUBLIC MPI::MPI_CXX)
