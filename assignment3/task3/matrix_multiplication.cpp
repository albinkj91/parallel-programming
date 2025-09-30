#include <iostream>
#include <chrono>
#include "omp.h"

#define DIM 2000

void matrix_multiplication(int **a, int **b, int **c, int dim) {
    #pragma omp parallel default(private) shared (a, b, c, dim, std::cout)
    #pragma omp for schedule(static)
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            c[i][j] = 0;
            for (int k = 0; k < dim; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void init_array(int **array, int dim) {
    for (int i = 0; i < dim; ++i) {
        array[i] = new int[dim];
        for (int j = 0; j < dim; j++) {
            array[i][j] = 0;
        }
    }
}

void fill_array(int **array, int dim) {
    for (int i = 0; i < dim; ++i) {
        array[i] = new int[dim];
        for (int j = 0; j < dim; j++) {
            array[i][j] = i * dim + j;
        }
    }
}

bool compare_arrays(int **a, int **b, int dim) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            if (a[i][j] != b[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void print_array(int **array, int dim) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            std::cout << array[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char** argv) {

    int **a = new int*[DIM];
    fill_array(a, DIM);

    int **b = new int*[DIM];
    fill_array(b, DIM);

    int **c_one_thread = new int*[DIM];
    init_array(c_one_thread, DIM);

    int **c = new int*[DIM];
    init_array(c, DIM);

    omp_set_num_threads(1);
    auto start = std::chrono::system_clock::now();
    matrix_multiplication(a, b, c_one_thread, DIM);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Finished single-threaded after " << duration.count() << "s\n";

    omp_set_num_threads(8);
    start = std::chrono::system_clock::now();
    matrix_multiplication(a, b, c, DIM);
    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << "Finished multi-threaded after " << duration.count() << "s\n";

    std::cout << "Multi-threaded is equal to single-threaded: " << (compare_arrays(c_one_thread, c, DIM) ? "True" : "False") << std::endl;

    return 0;
}