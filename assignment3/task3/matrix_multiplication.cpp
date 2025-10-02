#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include "omp.h"

#define DIM_ERROR 2000

void usage(char *program)
{
  std::cout << "Usage: " << program << " T dimI dimJ dimK C" << std::endl;
  std::cout << std::endl;
  std::cout << "  T:\tnumber of threads" << std::endl;
  std::cout << "  dimI:\theight of matrix a" << std::endl;
  std::cout << "  dimJ:\twidth of matrix b" << std::endl;
  std::cout << "  dimK:\twidth of matrix a and height of matrix b" << std::endl;
  std::cout << "  C:\tcollapse-level (1-3)" << std::endl;
}

void matrix_multiplication_collapse1(int **a, int **b, int **c, int dimI, int dimJ, int dimK) {
    #pragma omp parallel default(private) shared (a, b, c, dimI, dimJ, dimK, std::cout)
    #pragma omp for schedule(static)
    for (int i = 0; i < dimI; i++) {
        for (int j = 0; j < dimJ; j++) {
            c[i][j] = 0;
            for (int k = 0; k < dimK; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void matrix_multiplication_collapse2(int **a, int **b, int **c, int dimI, int dimJ, int dimK) {
    #pragma omp parallel default(private) shared (a, b, c, dimI, dimJ, dimK, std::cout)
    #pragma omp for schedule(static) collapse(2)
    for (int i = 0; i < dimI; i++) {
        for (int j = 0; j < dimJ; j++) {
            c[i][j] = 0;
            for (int k = 0; k < dimK; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void matrix_multiplication_collapse3(int **a, int **b, int **c, int dimI, int dimJ, int dimK) {
    #pragma omp parallel default(private) shared (a, b, c, dimI, dimJ, dimK, std::cout)
    #pragma omp for schedule(static) collapse(3)
    for (int i = 0; i < dimI; i++) {
        for (int j = 0; j < dimJ; j++) {
            for (int k = 0; k < dimK; k++) {
                #pragma omp critical
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void matrix_multiplication_collapse3_ERROR(int **a, int **b, int **c, int dimI, int dimJ, int dimK) {
    #pragma omp parallel default(private) shared (a, b, c, dimI, dimJ, dimK, std::cout)
    #pragma omp for schedule(static) collapse(3)
    for (int i = 0; i < dimI; i++) {
        for (int j = 0; j < dimJ; j++) {
            for (int k = 0; k < dimK; k++) {
                // #pragma omp critical // PRODUCES DATA RACES
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

/**
 * Initializes array with 0
 */
void init_array(int **array, int dimI, int dimJ) {
    // for (int i = 0; i < dimI; ++i) {
    //     array[i] = new int[dimJ];
    //     for (int j = 0; j < dimJ; j++) {
    //         array[i][j] = 0;
    //     }
    // }
    #pragma omp parallel for
    for (int i = 0; i < dimI; ++i) {
        array[i] = new int[dimJ];
    }

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < dimI; ++i) {
        for (int j = 0; j < dimJ; j++) {
            array[i][j] = 0;
        }
    }
}

/**
 * Initializes array with random values between 0 and 1000
 */
void fill_array_random(int **array, int dimI, int dimJ) {
    /**
     * We tried to use random numbers, but that makes the initialization time rather slow.
     * So, we decided to just use the current matrix position as the number. The values
     * are thus in ascending order, but for the matrix multiplication itself it doesn't
     * make a difference in terms of speed.
     */
    // #pragma omp parallel for
    // for (int i = 0; i < dimI; ++i) {
    //     array[i] = new int[dimJ];
    // }
    // srand(time(0));
    // unsigned int seed = rand();
    // #pragma omp parallel for collapse(2) shared(array)
    // for (int i = 0; i < dimI; ++i) {
    //     for (int j = 0; j < dimJ; j++) {
    //         array[i][j] = rand_r(&seed) % 1000;
    //     }
    // }

    #pragma omp parallel for
    for (int i = 0; i < dimI; ++i) {
        array[i] = new int[dimJ];
    }

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < dimI; ++i) {
        for (int j = 0; j < dimJ; j++) {
            array[i][j] = i * dimJ + j;
        }
    }
}

/**
 * Compares the arrays element-wise
 */
bool compare_arrays(int **a, int **b, int dimI, int dimJ) {
    for (int i = 0; i < dimI; ++i) {
        for (int j = 0; j < dimJ; ++j) {
            if (a[i][j] != b[i][j]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Prints the array
 */
void print_array(int **array, int dimI, int dimJ) {
    for (int i = 0; i < dimI; ++i) {
        for (int j = 0; j < dimJ; ++j) {
            std::cout << array[i][j] << " ";
        }
        std::cout << "\n";
    }
}

/**
 * Runs the benchmark depending on the collapse-level
 */
void run_benchmark(int** a, int** b, int** c, int num_of_threads, int dimI, int dimJ, int dimK, int collapse_level) {
    omp_set_num_threads(num_of_threads);
    auto start = std::chrono::system_clock::now();
    switch (collapse_level) {
        case -3:
            matrix_multiplication_collapse3_ERROR(a, b, c, dimI, dimJ, dimK);
            break;
        case 2:
            matrix_multiplication_collapse2(a, b, c, dimI, dimJ, dimK);
            break;
        case 3:
            matrix_multiplication_collapse3(a, b, c, dimI, dimJ, dimK);
            break;
        default:
            matrix_multiplication_collapse1(a, b, c, dimI, dimJ, dimK);
            break;
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Finished after " << duration.count() << "s with " << num_of_threads << " threads and collapse(" << collapse_level << ")\n";
}

/**
 * Just for demonstration that collapse(3) leads to data races without synchronisation
 */
void demonstrateError() {
    std::cout << "Starting demonstration with erroneous collapse(3). First, normal execution, can take some time\n";
    int **a = new int*[DIM_ERROR];
    fill_array_random(a, DIM_ERROR, DIM_ERROR);

    int **b = new int*[DIM_ERROR];
    fill_array_random(b, DIM_ERROR, DIM_ERROR);

    int **c_check = new int*[DIM_ERROR];
    init_array(c_check, DIM_ERROR, DIM_ERROR);

    int **c_error = new int*[DIM_ERROR];
    init_array(c_error, DIM_ERROR, DIM_ERROR);

    run_benchmark(a, b, c_check, 16, DIM_ERROR, DIM_ERROR, DIM_ERROR, 1);
    run_benchmark(a, b, c_error, 16, DIM_ERROR, DIM_ERROR, DIM_ERROR, -3);
    std::cout << "collapse(3) with erroneous code is equal to normal: " 
        << (compare_arrays(c_check, c_error, DIM_ERROR, DIM_ERROR) ? "True" : "False") 
        << std::endl;
}

/**
 * Prepares and runs benchmark
 */
void benchmark(int num_of_threads, int dimI, int dimJ, int dimK, int collapse_level) {
    // For debugging: Initialization of big matrices may take some time
    // std::cout << "Initializing matrices\n";
    int **a = new int*[dimI];
    fill_array_random(a, dimI, dimK);

    int **b = new int*[dimK];
    fill_array_random(b, dimK, dimJ);

    int **c = new int*[dimI];
    init_array(c, dimI, dimJ);

    // For debugging: Initialization of big matrices may take some time
    // std::cout << "Starting benchmark\n";
    run_benchmark(a, b, c, num_of_threads, dimI, dimJ, dimK, collapse_level);
}

int main(int argc, char** argv) {
    // auto start = std::chrono::system_clock::now();
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-e") == 0) {
            demonstrateError();
            return 0;
        }
    }

    if (argc != 6) {
        usage(argv[0]);
        return 1;
    }

    int num_of_threads, dimI, dimJ, dimK, collapse_level;

    try {
        num_of_threads = std::stoi(argv[1]);
        dimI = std::stoi(argv[2]);
        dimJ = std::stoi(argv[3]);
        dimK = std::stoi(argv[4]);
        collapse_level = std::stoi(argv[5]);
    } catch (const std::exception&) {
        usage(argv[0]);
        return 1;
    }

    if (collapse_level < 0) {
        std::cout << "Collapse-level must be between 1 and 3 (inclusive)\n";
        return 1;
    }

    benchmark(num_of_threads, dimI, dimJ, dimK, collapse_level);

    // auto end = std::chrono::system_clock::now();
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Total execution: " << duration.count() << "s\n";
    return 0;
}