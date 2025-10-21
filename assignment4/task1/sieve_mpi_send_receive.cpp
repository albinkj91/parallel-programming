#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <mpi.h>
using namespace std;

void usage(string const& program)
{
  std::cout << "Usage: " << program << "N" << std::endl;
  std::cout << std::endl;
  std::cout << "  N: max value" << std::endl;
  exit(1);
}

void main_process(int argc, char** argv, int numOfProcesses) {
    if(argc < 2)
        usage(argv[0]);

    istringstream ss{argv[1]};
    int32_t max{};
    ss >> max;
    if(ss.fail())
        usage(argv[0]);

    vector<int> nums{};
    vector<int> primes{};
    int start{2};
    generate_n(back_inserter(nums), max-1, [&start](){
                return start++;
            });

    double startTime = MPI_Wtime();
    int current_index{};
    int current_prime{nums.front()};
    while(current_prime*current_prime <= max)
    {
        primes.push_back(current_prime);
        int limit{static_cast<int>(sqrt(max))};
        for(int i = current_index+1; i < limit; ++i)
        {
            if(nums[i] % current_prime == 0)
                nums[i] = -1;
        }
        do
            current_prime = nums[++current_index];
        while(current_prime == -1 && current_index < nums.size());
    }

    start = primes.back() + 1;
    int totalSize = max + 1 - start;
    int chunkSize = totalSize / (numOfProcesses - 1);
    int numbersLeft = totalSize % (numOfProcesses - 1);

    for (int process = 1; process < numOfProcesses; ++process) {
        int end  = start + (process - 1 < numbersLeft ? chunkSize + 1 : chunkSize);
        int* start_end_numOfPrimes = new int[3] {start, end, static_cast<int>(primes.size())};
        MPI_Send(start_end_numOfPrimes, 3, MPI_INT, process, 0, MPI_COMM_WORLD);
        MPI_Send(primes.data(), primes.size(), MPI_INT, process, 1, MPI_COMM_WORLD);
        start = end;
    }

    int sum = primes.size();
    for (int process = 1; process < numOfProcesses; ++process) {
        int n;
        MPI_Recv(&n, 1, MPI_INT, process, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += n;
    }

    double endTime = MPI_Wtime();
    std::cout << sum << " primes found after " << (endTime - startTime) << "s\n";
}

void worker_process(int rank) {
    int start_end_numOfPrimes[3];
    MPI_Recv(start_end_numOfPrimes, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int* primes = new int[start_end_numOfPrimes[2]];
    MPI_Recv(primes, start_end_numOfPrimes[2], MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int start = start_end_numOfPrimes[0];
    int end = start_end_numOfPrimes[1];
    int numOfPrimes = start_end_numOfPrimes[2];

    int sum = 0;
    bool cont = false;
    for(int i = start; i < end; ++i)
    {
        cont = false;
        for(int j = 0; j < numOfPrimes; ++j)
        {
            if(i % primes[j] == 0) {
                cont = true;
                break;
            }
        }
        if (cont) continue;
        ++sum;
    }
    MPI_Send(&sum, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
}

int main(int argc, char* argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        main_process(argc, argv, size);
    } else {
        worker_process(rank);
    }

    MPI_Finalize();
    return 0;
}
