#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <tuple>
#include <random>
#include <functional>
#include <cstring>
#include "omp.h"


#define DIM 42000
#define MAX_CHUNK_SIZE 1<<13

void usage(char *program)
{
  std::cout << "Usage: " << program << " [-c]" << std::endl;
  std::cout << std::endl;
  std::cout << "  -c:\trun including single-threded column-based version (optional at own risk!!)" << std::endl;
}

// ROW-ORIENTED-GAUSS

void row_oriented_normal(int** A, int* b, int* x, int n) {
    for (int row = n - 1; row >= 0; --row) {
        x[row] = b[row];
        for (int col = row + 1; col < n; ++col) {
            x[row] -= A[row][col] * x[col];
        }
        x[row] /= A[row][row];
    }
}

void row_oriented_parallel(int** A, int* b, int* x, int n) {
    for (int row = n - 1; row >= 0; --row) {
        x[row] = b[row];
        int sum = 0;

        #pragma omp parallel for reduction(+:sum) schedule(runtime)
        for (int col = row + 1; col < n; ++col) {
            sum += A[row][col] * x[col];
        }
        x[row] -= sum;
        x[row] /= A[row][row];
    }
}

// COLUMN-ORIENTED-GAUSS

void col_oriented_normal(int** A, int* b, int* x, int n) {
    for (int row = 0; row < n; ++row) {
        x[row] = b[row];
    }
    for (int col = n - 1; col >= 0; --col) {
        x[col] /= A[col][col];
        for (int row = 0; row < col; ++row) {
            x[row] -= A[row][col] * x[col];
        }
    }
}

void col_oriented_parallel(int** A, int* b, int* x, int n) {
    #pragma omp parallel for schedule(runtime)
    for (int row = 0; row < n; ++row) {
        x[row] = b[row];
    }

    for (int col = n - 1; col >= 0; --col) {
        x[col] /= A[col][col];
        #pragma omp parallel for schedule(runtime)
        for (int row = 0; row < col; ++row) {
            x[row] -= A[row][col] * x[col];
        }
    }
}

// HELPER FUNCTIONS

void generate_safe_LES(int** A, int* b, int* x, int dim) {
    #pragma omp parallel 
    {
        #pragma omp single
        std::cout << "Running with " << omp_get_num_threads() << " Threads" << std::endl;

        std::mt19937 gen(time(0) + omp_get_thread_num());
        std::uniform_int_distribution<int> dist_x(-20, 20);
        #pragma omp for
        for (int i = 0; i < dim; ++i) {
            x[i] = dist_x(gen);
        }
    }

    #pragma omp parallel
    {
        std::mt19937 gen(time(0) + omp_get_thread_num());
        std::uniform_int_distribution<int> dist_off(-20, 20);
        std::uniform_int_distribution<int> dist_on(1, 10);
        std::uniform_int_distribution<int> dist_dec(0, 1);
        #pragma omp for
        for (int i = 0; i < dim; ++i) {
            A[i] = new int[dim];
            for (int j = 0; j < dim; ++j) {
                if (j < i) A[i][j] = 0;
                else if (i == j) A[i][j] = dist_dec(gen) == 0 ? -dist_on(gen) : dist_on(gen);
                else A[i][j] = dist_off(gen);
            }
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < dim; ++i) {
        b[i] = 0;
        for (int j = 0; j < dim; ++j) {
            b[i] += A[i][j] * x[j];
        }
    }
}

void print_array(int* array, int dim) {
    for (int i = 0; i < dim; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << "\n";
}

void print_matrix(int** array, int dim) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            std::cout << array[i][j] << " ";
        }
        std::cout << "\n";
    }
}

std::tuple<int, int, int> compare_arrays(int* a1, int* a2, int dim) {
    for (int i = 0; i < dim; ++i) {
        if (a1[i] != a2[i]) {
            return std::make_tuple(i, a1[i], a2[i]);
        }
    }

    return std::make_tuple(-1, 0, 0);
}

void run_benchmark(std::function<void(int**, int*, int*, int)> backward_substituition, 
                    int** A, int* b, int* x, int* x_expected, int dim, std::string name) {
    auto start = std::chrono::system_clock::now();
    backward_substituition(A, b, x, dim);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Finished \"" << name << "\" after " << duration.count() << "s\n";

    /**
     * The following code was used to determine if the parallelized versions
     * produced correct results
     */
    // int index;
    // double actual, expected;
    // std::tie(index, expected, actual) = compare_arrays(x_expected, x, dim);
    // if (index == -1) {
    //     std::cout << "Solution array is as expected\n";
    // } else {
    //     std::cout << "Expected " << expected << " at index " << index << ", but was " << actual << std::endl;
    // }
}

void run(int** A, int* b, int* x_expected, int dim, omp_sched_t schedule, std::string sched_name) {
    /**
     * since both backward substituition algorithms overwrite the array x as the very first step,
     * one can reuse the same arrays instead of repeatedly creating new ones
     */
    int* x1 = new int[dim];
    int* x2 = new int[dim];

    std::cout << "\nrow-oriented parallel " << sched_name << ":\n";
    for (int i = 1; i <= MAX_CHUNK_SIZE; i *= 2) {
        omp_set_schedule(schedule, i);
        run_benchmark(row_oriented_parallel, A, b, x1, x_expected, dim, sched_name + " " + std::to_string(i));
    }

    std::cout << "\ncolumn-oriented parallel " << sched_name << ":\n";

    for (int i = 1; i <= MAX_CHUNK_SIZE; i *= 2) {
        omp_set_schedule(schedule, i);
        run_benchmark(col_oriented_parallel, A, b, x2, x_expected, dim, sched_name + " " + std::to_string(i));
    }

    delete[] x1;
    delete[] x2;
}

void benchmark(bool run_col_normal) {
    int** A = new int*[DIM];
    int* b = new int[DIM];
    int* x_expected = new int[DIM];
    int* x1 = new int[DIM];
    int* x2 = new int[DIM];

    auto start = std::chrono::system_clock::now();
    generate_safe_LES(A, b, x_expected, DIM);
    auto middle = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = middle - start;
    std::cout << "Init done after " << duration.count() << "s\n\n";

    // single-threaded versions
    run_benchmark(row_oriented_normal, A, b, x1, x_expected, DIM, "row-oriented normal");
    if (run_col_normal) {
        run_benchmark(col_oriented_normal, A, b, x2, x_expected, DIM, "column-oriented normal");
    } else {
        std::cout << "Skipping single-threaded column-oriented version because it takes tooooooo long\n";
    }

    // parallel executions
    run(A, b, x_expected, DIM, omp_sched_static, "static");
    run(A, b, x_expected, DIM, omp_sched_dynamic, "dynamic");
    run(A, b, x_expected, DIM, omp_sched_guided, "guided");
}

int main(int argc, char** argv) {
    bool run_col_normal = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-c") == 0) {
            run_col_normal = true;
        }
    }

    benchmark(run_col_normal);
    return 0;
}