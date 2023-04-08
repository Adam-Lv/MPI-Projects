CXX_FLAGS = -std=c++14 -g -Wall
CC = mpicxx
OUT_DIR = out
#INCLUDE_DIR = /usr/local/openmpi/include
#LIB_DIR = /usr/local/openmpi/lib

.PHONY:
all: hello_world calculate_pi prime_number matrix_multiplication

hello_world: mpi_hello_world.cpp
	$(CC) $(CXX_FLAGS) mpi_hello_world.cpp -o $(OUT_DIR)/hello_world

calculate_pi: mpi_calculate_pi.cpp
	$(CC) $(CXX_FLAGS) mpi_calculate_pi.cpp -o $(OUT_DIR)/calculate_pi

prime_number: mpi_prime_number.cpp
	$(CC) $(CXX_FLAGS) mpi_prime_number.cpp -o $(OUT_DIR)/prime_number

matrix_multiplication: mpi_matrix_multiplication.cpp
	$(CC) $(CXX_FLAGS) mpi_matrix_multiplication.cpp src/matrix_multiplication.cpp -o $(OUT_DIR)/matrix_multiplication

.PHONY:
clean:
	rm -rf $(OUT_DIR)/*
