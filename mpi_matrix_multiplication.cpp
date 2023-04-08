#include <iostream>
#include <chrono>
#include <iomanip>
#include "include/matrix_multiplication.h"


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int m_size;
    // 处理命令行输入
    if (argc == 1) m_size = 16;
    else if (argc == 2) m_size = stoi(argv[1]);
    else {
        cerr << "Error: invalid arguments." << endl;
        exit(1);
    }
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);
    double mpi_start = MPI_Wtime();

    MatrixMultiplication mm(size, rank, m_size);
    // cout << mm << endl;
    mm.calculation_init();
    // cout << "Rank " << rank << " initialization completed." << endl;
    mm.calculation_execute();
    // cout << "Rank " << rank << " execution completed." << endl;
    mm.calculation_gather();
    if (rank == 0) {
        double mpi_end = MPI_Wtime();
        // MatrixMultiplication::print_matrix(mm.A);
        // MatrixMultiplication::print_matrix(mm.B);
        MatrixMultiplication::print_matrix(mm.C);
        auto naive_start = chrono::high_resolution_clock::now();
        auto C = mm.multiplicationNaive();
        auto naive_end = chrono::high_resolution_clock::now();
        MatrixMultiplication::print_matrix(C);
        int result = 1;
        for (int i = 0; i < m_size; i++) {
            for (int j = 0; j < m_size; j++) {
                result *= (mm.C[i][j] - C[i][j]) == 0;
            }
        }
        auto duration = chrono::duration_cast<chrono::microseconds>(naive_end - naive_start);
        cout << "Process num: " << size << endl
             << "Matrix size: " << m_size << endl
             << "The multiplication result is " << boolalpha << (result == 1) << endl
             << "Naive method time cost: " << fixed << setprecision(3) << ((double) duration.count()) / 1000 << " ms" << endl
             << "Cannon algorithm time cost: " << (mpi_end - mpi_start) * 1e3 << " ms" << endl;
    }
    MPI_Finalize();
    return 0;
}
