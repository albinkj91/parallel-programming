#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <omp.h>
using namespace std;

void usage(string const& program)
{
  std::cout << "Usage: " << program << "N" << std::endl;
  std::cout << std::endl;
  std::cout << "  N: max value" << std::endl;
  exit(1);
}

int main(int argc, char* argv[])
{
    if(argc < 2)
        usage(argv[0]);

    istringstream ss{argv[1]};
    int32_t max{};
    ss >> max;
    if(ss.fail())
        usage(argv[0]);

    vector<int32_t> nums{};
    vector<uint32_t> primes{};
    uint32_t start{2};
    generate_n(back_inserter(nums), max-1, [&start](){
                return start++;
            });

    uint32_t current_index{};
    int32_t current_prime{nums.front()};
    auto start_time{omp_get_wtime()};
    while(current_prime*current_prime <= max)
    {
        primes.push_back(current_prime);
        uint32_t limit{static_cast<uint32_t>(sqrt(max))};
        for(uint32_t i = current_index+1; i < limit; ++i)
        {
            if(nums[i] % current_prime == 0)
                nums[i] = -1;
        }
        do
            current_prime = nums[++current_index];
        while(current_prime == -1 && current_index < nums.size());
    }

    start = static_cast<uint64_t>(primes.back()) + 1;

    #pragma omp parallel for collapse(2) shared(nums, primes) schedule(static)
    for(uint64_t i = start; i < nums.size(); ++i)
    {
        for(uint64_t j = 0; j < primes.size(); ++j)
        {
            if(nums[i] % primes[j] == 0) {
                nums[i] = -1;
            }

        }
    }

    int sum = 0;
    for (std::size_t i = 0; i < nums.size(); ++i) {
        if (nums[i] != -1) {
            ++sum;
        }
    }

    auto end_time{omp_get_wtime()};
    cout << sum << " primes found after " << (end_time - start_time) << "s." << endl;
}
