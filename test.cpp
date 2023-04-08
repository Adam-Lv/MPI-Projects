//
// Created by 初夏之黎明 on 2023/4/2.
//
#include <iostream>
#include <vector>
#include <random>
//#include "mpi.h"
using namespace std;

vector<vector<int>> generateMatrix(int n) {
    vector<vector<int>> matrix(n, vector<int>(n));
    random_device rd;
    mt19937 gen(rd());  // 使用Mersenne Twister算法
    uniform_int_distribution<> dis(-100, 100);  // 生成-100到100之间的均匀分布

    for (auto &line: matrix) {
        for (auto &item: line) {
            item = dis(gen);
        }
    }
    return matrix;
}

int main(int argc, char** argv) {
//    MPI_Init(&argc, &argv);
//    int rank, size;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//
//    std::vector<int> local_vec(10); // 每个进程定义一个长度为10的本地std::vector<int>
//    for (int i = 0; i < local_vec.size(); ++i) {
//        local_vec[i] = rank * local_vec.size() + i; // 每个进程往自己的std::vector<int>中填充数据
//    }
//
//    std::vector<int> global_vec; // 定义一个全局std::vector<int>，用于接收来自其他进程的数据
//    if (rank == 0) {
//        global_vec.resize(local_vec.size() * size); // 进程0负责合并，因此需要事先分配足够的空间
//    }
//
//    MPI_Gather(local_vec.data(), local_vec.size(), MPI_INT,
//               global_vec.data(), local_vec.size(), MPI_INT,
//               0, MPI_COMM_WORLD); // 使用MPI_Gather函数将每个进程中的std::vector<int>发送到进程0并合并到全局std::vector<int>中
//
//    if (rank == 0) {
//        std::cout << "Global vector: ";
//        for (int i = 0; i < global_vec.size(); ++i) {
//            std::cout << global_vec[i] << " "; // 进程0输出合并后的全局std::vector<int>
//        }
//        std::cout << std::endl;
//    }
//
//    MPI_Finalize();
//    return 0;
    auto matrix = generateMatrix(10);
    cout << matrix[0][0] << endl;
    return 0;
}

//int main(int argc, char* argv[])
//{
//    int rank, size;
//    MPI_Init(&argc, &argv);
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//
//    srand(time(NULL) + rank); // 每个进程使用不同的种子生成随机数
//
//    int rand_num = rand() % 100; // 生成0-99之间的随机数
//    cout << "rank " << rank << ": " << rand_num << endl;
//
//    int min_num; // 所有进程最小值的变量
//
//    MPI_Reduce(&rand_num, &min_num, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
//    // rank为0的进程汇总所有的随机数并找到最小值
//
//    MPI_Bcast(&min_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
//    // rank为0的进程将最小值广播给所有进程
//
//    if (rank == size - 1) // rank最高的进程打印最小值
//    {
//        cout << "The minimum number is: " << min_num << endl;
//    }
//
//    MPI_Finalize();
//    return 0;
//}