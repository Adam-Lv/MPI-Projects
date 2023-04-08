#include <iostream>
#include "mpi.h"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int n;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    int r;
    MPI_Comm_rank(MPI_COMM_WORLD, &r);
    char h[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(h, &name_len);
    std::cout << "Hello, World from process " << r << " of " << n << " on " << h << std::endl;
    MPI_Finalize();
    return 0;
}
