#ifndef MPI_MATRIX_MULTIPLICATION_H
#define MPI_MATRIX_MULTIPLICATION_H

#include "mpi.h"
#include <vector>
#include <iostream>

using namespace std;

typedef vector<vector<int>> squareMatrixInt;

enum Tag {
    tagA = 1,
    tagB,
    tagC
};

class MatrixMultiplication {
public:
    /**
     * @brief 初始矩阵A
     */
    squareMatrixInt A;

    /**
     * @brief 初始矩阵B
     */
    squareMatrixInt B;

    /**
     * @brief 结果矩阵C
     */
    squareMatrixInt C;

    /**
     * @brief MPI进程数量
     */
    int size;

    /**
     * @brief 当前进程rank
     */
    int rank;

    /**
     * @brief 矩阵的size
     */
    int m_size;

    MatrixMultiplication(int size, int rank, int m_size);

    /**
     * @brief 生成一个整型方阵，数值从-100到100，尺寸为n * n。
     * @param randomInit 是否随机初始化
     * @param n 方阵的尺寸
     * @return 整型方阵
     */
    static squareMatrixInt generateMatrix(bool randomInit, int n);

    /**
     * @brief 进程0生成A、B矩阵，并将初始计算块分配给所有其他进程
     */
    void calculation_init();

    /**
     * @brief 所有计算节点执行计算过程与移位传递过程
     */
    void calculation_execute();

    /**
     * @brief 进行最后一次小矩阵乘法运算，并在完成后做结果聚合
     */
    void calculation_gather();

    /**
     * @brief 用最传统的方法计算A*B
     * @return A*B的结果
     */
    squareMatrixInt multiplicationNaive();

    static void print_matrix(vector<vector<int>> &matrixInt);

    friend std::ostream &operator<<(std::ostream &os, const MatrixMultiplication &obj);

private:
    /**
     * @brief A的计算存储矩阵
     */
    squareMatrixInt mA;

    /**
     * @brief B的计算存储矩阵
     */
    squareMatrixInt mB;

    /**
     * @brief C的计算存储矩阵
     */
    squareMatrixInt mC;

    /**
     * @brief 计算矩阵的size, block_size
     */
    int b_size;

    /**
     * @brief 每条矩阵边的处理器数量
     */
    int num_of_process_per_side;

    /**
     * @brief 当前进程计算块的行编号
     */
    int line;

    /**
     * @brief 当前进程计算块的列编号
     */
    int column;

    /**
     * @brief 发送mA的目的进程
     */
    int A_dst_rank;

    /**
     * @brief 发送mB的目的进程
     */
    int B_dst_rank;

    /**
     * @brief 接收mA的源进程
     */
    int A_src_rank;

    /**
     * @brief 接收mB的源进程
     */
    int B_src_rank;

    /**
     * @brief 在calculation_init阶段，由进程0给其他进程发送初始矩阵块的函数
     * @param dst_rank 目标进程号
     * @param location 矩阵块所在大矩阵的坐标，其中location[0]、location[1]代表了矩阵块的左上角行列坐标，
     * location[2]、location[3]代表了矩阵块的右下角的行列坐标
     * @param tag 发送的消息标签，用于区分发送的是矩阵A(tagA)还是B(tagB)
     */
    void sendBlock(int dst_rank, const vector<int> &location, Tag tag);

    /**
     * @brief 在其他阶段（除了calculation_init），由当前进程发给目的进程计算矩阵块的函数。
     * 由于矩阵本身是二维嵌套的std::vector<>，在使用MPI发送时存在诸多不便，因此在发送前将矩
     * 阵展平为一维vector方便发送
     * @param dst_rank 目标进程号
     * @param tag 发送消息的标签，用于区分发送的是矩阵mA（tagA）、矩阵mB（tagB）还是矩阵mC（tagC）
     * @param matrix 要发送的矩阵变量的引用
     * @return 堆区request变量，可以用于后续进行消息同步。
     * @attention 返回变量为堆区变量，若后续不再使用请用delete销毁以防止内存泄露
     */
    MPI_Request *sendBlock(int dst_rank, Tag tag, squareMatrixInt &matrix) const;

    /**
     * @brief 用于所有阶段接收由sendBlock发送的矩阵块内容，注意，接收内容为一维vector
     * @param src_rank 消息源进程号
     * @param tag 接收消息的标签，用于区分接收的是矩阵mA（tagA）、矩阵mB（tagB）还是矩阵mC（tagC）
     * @param buffer 用于存储接收到的数据的临时数据缓冲区
     * @return 堆区request变量，可以用于后续进行消息同步。
     * @attention 返回变量为堆区变量，若后续不再使用请用delete销毁以防止内存泄露
     */
    static MPI_Request *receiveBlock(int src_rank, Tag tag, vector<int> &buffer);
};

#endif //MPI_MATRIX_MULTIPLICATION_H
