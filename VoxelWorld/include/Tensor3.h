
template <class T, int dim1, int dim2, int dim3> class Tensor3 {
    T data[dim1][dim2][dim3];

public:
    T& operator()(const int x, const int y, const int z) {
        return this->data[x][y][z];
    }
};
