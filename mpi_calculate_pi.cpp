#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) {
    int num_procs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    const int iterations = 1000000;
    double pi = 0.0;
    double x;
    for (int i = rank; i < iterations; i += num_procs) {
        x = (i + 0.5) / iterations;
        pi += 4.0 / (1.0 + x * x);
    }

    double total_pi;
    MPI_Reduce(&pi, &total_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        cout.precision(15);
        cout << "Pi is approximately: " << total_pi / iterations << endl;
        cout << "Total time is: " << end - start << "s."<< endl;
    }

    MPI_Finalize();
    return 0;
}