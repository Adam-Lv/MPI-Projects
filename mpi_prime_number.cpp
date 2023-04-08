#include <iostream>
#include "mpi.h"
#include <vector>
#include <map>
#include <algorithm>


void remove0FromVector(std::vector<int> &vec) {
    vec.erase(std::remove(vec.begin(), vec.end(), 0), vec.end());
}

void printVectorInt(const std::vector<int> &vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i != vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Barrier(MPI_COMM_WORLD);
    int n;
    if (argc == 1) n = 100;
    else if (argc == 2) n = std::stoi(argv[1]);
    else {
        std::cerr << "Error: invalid arguments." << std::endl;
        exit(1);
    }

    double start_time = MPI_Wtime();
    // 每个进程需要处理的数的下界
    int lower_bound = rank * (n - 2) / size + 2;
    // 每个进程需要处理的数的上界
    int upper_bound = (rank + 1) * (n - 2) / size + 2;
    // k的初始值
    int k = 2;
    // 用于标记一个数是否marked
    std::map<int, bool> marked;
    for (int i = lower_bound; i <= upper_bound; i++)
        marked.insert(std::make_pair(i, false));
    // 用于存储全局的质数，并提前分配n个空间
    std::vector<int> global_primes(n);
    // 用于存储局部的质数
    std::vector<int> local_primes;
    while (k * k <= n) {
        // 当前循环下当前进程需要检查是否要标记它的第一个数
        int start;
        if (lower_bound <= k * k) {
            start = k * k;
        } else {
            start = lower_bound % k == 0 ? lower_bound : ((lower_bound / k) + 1) * k;
        }
        // 标记k的倍数
        for (int i = start; i <= upper_bound; i += k)
            marked[i] = true;
        // 找到最小的比k大的没有被标记的数
        int local_smallest_unmarked;
        for (const auto item: marked) {
            if (!item.second && item.first > k) {
                local_smallest_unmarked = item.first;
                break;
            }
        }
        // 进程0找到全局最小，并广播给所有进程
        MPI_Reduce(&local_smallest_unmarked, &k, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
        MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    // 循环结束时没有被标记的数就是质数
    for (const auto &item: marked) {
        if (!item.second)
            local_primes.push_back(item.first);
    }
    // 进程0汇总所有质数
    MPI_Gather(local_primes.data(), local_primes.size(), MPI_INT,
               global_primes.data(), local_primes.size(), MPI_INT,
               0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    remove0FromVector(global_primes);
    if (rank == 0) {
        printVectorInt(global_primes);
        std::cout.precision(15);
        std::cout << "Totol time: " << end_time - start_time << "s" << std::endl;
    }
    MPI_Finalize();
    return 0;
}