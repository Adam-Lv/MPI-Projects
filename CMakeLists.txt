cmake_minimum_required(VERSION 3.22)
project(MPI)

include_directories(/usr/local/openmpi/include)
link_directories(/usr/local/openmpi/lib)
link_libraries(mpi)
set(CMAKE_CXX_STANDARD 20)

add_executable(hello_world mpi_hello_world.cpp)
add_executable(calculate_pi mpi_calculate_pi.cpp)
add_executable(prime_number mpi_prime_number.cpp)
add_executable(test test.cpp)
add_executable(matrix_multiplication mpi_matrix_multiplication.cpp src/matrix_multiplication.cpp)
