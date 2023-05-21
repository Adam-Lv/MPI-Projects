#include "../include/matrix_multiplication.h"
#include <random>
#include <cmath>

void printVectorInt(vector<int> &vec) {
    cout << "[";
    for (int i = 0; i < vec.size() - 1; i++) {
        cout << vec[i] << ", ";
    }
    cout << vec.back() << "]\n";
}

MatrixMultiplication::MatrixMultiplication(int size, int rank, int m_size) {
    this->size = size;
    this->rank = rank;
    this->m_size = m_size;
    b_size = m_size / (int) sqrt(size);
    num_of_process_per_side = m_size / b_size;
    line = rank / num_of_process_per_side;
    column = rank % num_of_process_per_side;
    mA = generateMatrix(false, b_size);
    mB = generateMatrix(false, b_size);
    mC = generateMatrix(false, b_size);
    A_dst_rank = line * num_of_process_per_side + (column + num_of_process_per_side - 1) % num_of_process_per_side;
    B_dst_rank = ((line + num_of_process_per_side - 1) % num_of_process_per_side) * num_of_process_per_side + column;
    A_src_rank = line * num_of_process_per_side + (column + 1) % num_of_process_per_side;
    B_src_rank = ((line + num_of_process_per_side + 1) % num_of_process_per_side) * num_of_process_per_side + column;
}

squareMatrixInt MatrixMultiplication::generateMatrix(bool randomInit, int n) {
    squareMatrixInt matrix(n, vector<int>(n));
    if (randomInit) {
        // 设置随机数的前置操作
        random_device rd;
        mt19937 gen(rd());  // 使用Mersenne Twister算法
        uniform_int_distribution<> dis(-100, 100);  // 生成-100到100之间的均匀分布

        for (auto &m_line: matrix) {
            for (auto &item: m_line) {
                item = dis(gen);
            }
        }
    }
    return matrix;
}

void MatrixMultiplication::sendBlock(int dst_rank, const vector<int> &location, Tag tag) {
    MPI_Request request;
    vector<int> send_buffer(b_size * b_size);
    int k = 0;
    for (int i = location[0]; i <= location[2]; i++) {
        for (int j = location[1]; j <= location[3]; j++) {
            if (tag == tagA) {
                send_buffer[k++] = A[i][j];
            } else {
                send_buffer[k++] = B[i][j];
            }
        }
    }
    MPI_Isend(send_buffer.data(), b_size * b_size, MPI_INT,
              dst_rank, tag, MPI_COMM_WORLD, &request);
}

MPI_Request MatrixMultiplication::sendBlock(int dst_rank, Tag tag, squareMatrixInt &matrix) const {
    MPI_Request request;
    vector<int> send_buffer(b_size * b_size);
    int k = 0;
    for (int i = 0; i < b_size; i++) {
        for (int j = 0; j < b_size; j++) {
            send_buffer[k++] = matrix[i][j];
        }
    }
    MPI_Isend(send_buffer.data(), b_size * b_size, MPI_INT,
              dst_rank, tag, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request MatrixMultiplication::receiveBlock(int src_rank, Tag tag, vector<int> &buffer) {
    MPI_Request request;
    MPI_Irecv(buffer.data(), (int) buffer.size(), MPI_INT,
              src_rank, tag, MPI_COMM_WORLD, &request);
    return request;
}

void MatrixMultiplication::calculation_init() {
    // rank 0需要生成初始数据并进行初始分发
    if (rank == 0) {
        A = generateMatrix(true, m_size);
        B = generateMatrix(true, m_size);
        C = generateMatrix(false, m_size);
        for (int i = 0; i < b_size; i++) {
            for (int j = 0; j < b_size; j++) {
                mA[i][j] = A[i][j];
                mB[i][j] = B[i][j];
            }
        }
//        cout << "A = ";
//        print_matrix(A);
//        cout << "B = ";
//        print_matrix(B);
//        cout << "-------------------------------------------------------------\n";
//        cout << "Initialization result:\n";
//        cout << "Rank 0:\nmA = ";
//        print_matrix(mA);
//        cout << "mB = ";
//        print_matrix(mB);

        // 接收rank初始应获得的数据块坐标
        vector<int> block_tag(2);
        // 块在矩阵中左上角和右下角点的坐标
        vector<int> location(4);
        for (int dst_rank = 1; dst_rank < size; dst_rank++) {  // i即为rank
            // 接收rank所负责计算的行列块坐标
            int i_line = dst_rank / num_of_process_per_side;
            int i_column = dst_rank % num_of_process_per_side;
            // 初始A块的块坐标
            block_tag[0] = i_line % num_of_process_per_side;
            block_tag[1] = (i_column + i_line) % num_of_process_per_side;
            // 初始A块的实际坐标
            location[0] = block_tag[0] * b_size;
            location[1] = block_tag[1] * b_size;
            location[2] = location[0] + b_size - 1;
            location[3] = location[1] + b_size - 1;
            sendBlock(dst_rank, location, tagA);

            // 初始B块的块坐标
            block_tag[1] = i_column % num_of_process_per_side;
            block_tag[0] = (i_line + i_column) % num_of_process_per_side;
            // 初始B块的实际坐标
            location[0] = block_tag[0] * b_size;
            location[1] = block_tag[1] * b_size;
            location[2] = location[0] + b_size - 1;
            location[3] = location[1] + b_size - 1;
            sendBlock(dst_rank, location, tagB);
        }
    } else {  // 其他rank需要接收数据
        int buffer_size = b_size * b_size;
        MPI_Request requestA, requestB;
        MPI_Status status;
        vector<int> r_bufferA(buffer_size);
        vector<int> r_bufferB(buffer_size);
        MPI_Irecv(r_bufferA.data(), buffer_size, MPI_INT, 0, tagA, MPI_COMM_WORLD, &requestA);
        MPI_Irecv(r_bufferB.data(), buffer_size, MPI_INT, 0, tagB, MPI_COMM_WORLD, &requestB);
        MPI_Wait(&requestA, &status);
        MPI_Wait(&requestB, &status);
        for (int i = 0; i < buffer_size; i++) {
            int j = i / b_size;
            int k = i % b_size;
            mA[j][k] = r_bufferA[i];
            mB[j][k] = r_bufferB[i];
        }
//        cout << "Rank " << rank << ":\nmA = ";
//        print_matrix(mA);
//        cout << "mB = ";
//        print_matrix(mB);
    }
}

void MatrixMultiplication::calculation_execute() {
    // 移动的次数
    int count = num_of_process_per_side - 1;
    int buffer_size = b_size * b_size;
    MPI_Request requestArray[4];
    MPI_Status statusArray[4];
    vector<int> r_bufferA(buffer_size), r_bufferB(buffer_size);
    while (count--) {

        requestArray[0] = receiveBlock(A_src_rank, tagA, r_bufferA);
        requestArray[1] = receiveBlock(B_src_rank, tagB, r_bufferB);
        requestArray[2] = sendBlock(A_dst_rank, tagA, mA);
        requestArray[3] = sendBlock(B_dst_rank, tagB, mB);
        // 计算mC = mA * mB
        for (int i = 0; i < b_size; i++) {
            for (int j = 0; j < b_size; j++) {
                for (int k = 0; k < b_size; k++) {
                    mC[i][j] += mA[i][k] * mB[k][j];
                }
            }
        }
        // 等待发送接收完毕
        MPI_Waitall(4, requestArray, statusArray);
        // 将收到的数据存入mA, mB以进行下一次计算
        for (int i = 0; i < buffer_size; i++) {
            int j = i / b_size;
            int k = i % b_size;
            mA[j][k] = r_bufferA[i];
            mB[j][k] = r_bufferB[i];
        }
//        cout << "-------------------------------------------------------------\n";
//        cout << "Iteration " << num_of_process_per_side - count - 1 <<": rank " << rank << " received mA and mB:\nmA = ";
//        print_matrix(mA);
//        cout << "mB = ";
//        print_matrix(mB);
    }
}

void MatrixMultiplication::calculation_gather() {
    // 计算mC = mA * mB
    for (int i = 0; i < b_size; i++) {
        for (int j = 0; j < b_size; j++) {
            for (int k = 0; k < b_size; k++) {
                mC[i][j] += mA[i][k] * mB[k][j];
            }
        }
    }
    if (rank != 0) {
        auto request = sendBlock(0, tagC, mC);
        MPI_Status status;
        MPI_Wait(&request, &status);
    } else {
        // 先把自己的mC放进C
        for (int i = 0; i < b_size; i++) {
            for (int j = 0; j < b_size; j++) {
                C[i][j] = mC[i][j];
            }
        }
        // 接收其他进程发送的结果
        MPI_Status status;
        vector<MPI_Request> requestList(size - 1, nullptr);
        vector<vector<int>> bufferList(size - 1, vector<int>(b_size * b_size));
        for (int src_rank = 1; src_rank < size; src_rank++) {
            auto request = receiveBlock(src_rank, tagC, bufferList[src_rank - 1]);
            requestList[src_rank - 1] = request;
        }
        // 等待接收完毕
        for (auto request: requestList) {
            MPI_Wait(&request, &status);
        }
        int count = 1;
        vector<int> location(4);
        for (vector<int> &buffer: bufferList) {
            // 计算块坐标
            int i_line = count / num_of_process_per_side;
            int i_column = count % num_of_process_per_side;
            location[0] = i_line * b_size;
            location[1] = i_column * b_size;
            location[2] = location[0] + b_size - 1;
            location[3] = location[1] + b_size - 1;
            // buffer放入C中对应的块
            int i = 0;
            for (int j = location[0]; j <= location[2]; j++) {
                for (int k = location[1]; k <= location[3]; k++) {
                    C[j][k] = buffer[i++];
                }
            }
            count++;
        }
    }
}

squareMatrixInt MatrixMultiplication::multiplicationNaive() {
    auto result = generateMatrix(false, m_size);
    for (int i = 0; i < m_size; i++) {
        for (int j = 0; j < m_size; j++) {
            for (int k = 0; k < m_size; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

void MatrixMultiplication::print_matrix(squareMatrixInt &matrixInt) {
    cout << "Matrix(\n";
    for (int i = 0; i < (int) matrixInt.size(); ++i) {
        cout << "\t[";
        for (int j = 0; j < (int) matrixInt[i].size(); ++j) {
            cout << matrixInt[i][j];
            if (j < (int) matrixInt[i].size() - 1) {
                cout << " ";
            }
        }
        cout << "]";
        if (i < (int) matrixInt.size() - 1) {
            cout << endl;
        }
    }
    cout << "\n)" << endl;
}

std::ostream &operator<<(ostream &os, const MatrixMultiplication &obj) {
    os << "MatrixMultiplication(\n\tsize = " << obj.size
       << ",\n\trank = " << obj.rank
       << ",\n\tm_size = " << obj.m_size
       << ",\n\tb_size = " << obj.b_size
       << ",\n\tnum_of_process_per_side = " << obj.num_of_process_per_side
       << ",\n\tline = " << obj.line
       << ",\n\tcolumn = " << obj.column
       << ",\n\tA_dst_rank = " << obj.A_dst_rank
       << ",\n\tB_dst_rank = " << obj.B_dst_rank
       << ",\n\tA_src_rank = " << obj.A_src_rank
       << ",\n\tB_src_rank = " << obj.B_src_rank
       << "\n)";
    return os;
}
