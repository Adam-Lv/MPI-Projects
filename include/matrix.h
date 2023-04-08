#ifndef MPI_MATRIX_H
#define MPI_MATRIX_H

#include <vector>

using namespace std;

class Matrix : public vector<int> {
public:
    Matrix() = default;
    Matrix(int lines, int columns) : vector<int>(lines * columns), lines(lines), columns(columns) {}

    int &operator()(int i, int j) { return (*this)[i * columns + j]; }

    const int &operator()(int i, int j) const { return (*this)[i * columns + j]; }

    int &operator()(int i) { return (*this)[i]; }

    const int &operator()(int i) const { return (*this)[i]; }

private:
    int lines = 0;
    int columns = 0;
};

#endif //MPI_MATRIX_H
