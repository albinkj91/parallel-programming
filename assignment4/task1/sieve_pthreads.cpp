#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <thread>
#include <chrono>
using namespace std;

void sieve_chunk(int32_t const start, int32_t const end, vector<int32_t> const& primes, int* result)
{
    int sum = 0;
    bool cont = false;
    for (int i = start; i < end; ++i) {
        cont = false;
        for (auto prime : primes) {
            if (i % prime == 0) {
                cont = true;
                break;
            }
        }
        if (cont) continue;
        ++sum;
    }
    *result = sum;
}

void usage(string const& program)
{
  std::cout << "Usage: " << program << " T N" << std::endl;
  std::cout << std::endl;
  std::cout << "  N: max value" << std::endl;
  exit(1);
}

int main(int argc, char* argv[])
{
    if(argc < 3)
        usage(argv[0]);

    istringstream ss{argv[1]};
    int32_t thread_count{};
    ss >> thread_count;
    if(ss.fail())
        usage(argv[0]);

    ss = istringstream{argv[2]};
    int32_t max{};
    ss >> max;
    if(ss.fail())
        usage(argv[0]);

    vector<int32_t> nums{};
    vector<int32_t> primes{};
    int32_t start{2};
    generate_n(back_inserter(nums), max-1, [&start](){
                return start++;
            });

    uint32_t current_index{};
    int32_t current_prime{nums.front()};
    auto start_time{chrono::steady_clock::now()};
    while(current_prime*current_prime <= max)
    {
        primes.push_back(current_prime);
        for(size_t i{current_index+1}; i < sqrt(max); ++i)
        {
            if(nums[i] % current_prime == 0) {
                nums[i] = -1;
            }
        }
        do
            current_prime = nums.at(++current_index);
        while(current_prime == -1 && current_index < nums.size());
    }

    start = primes.back() + 1;
    int totalSize = max + 1 - start;
    int chunkSize = totalSize / thread_count;
    int remainder = totalSize % thread_count;
    vector<thread> threads{};
    int* results = new int[thread_count];

    for(int i = 0; i < thread_count; ++i)
    {
        int end = start + (i - 1 < remainder ? chunkSize + 1 : chunkSize);
        threads.push_back(thread{
                sieve_chunk,
                start,
                end,
                ref(primes),
                &results[i]});
        start = end;
    }

    for(auto& thread : threads)
        thread.join();

    int sum = primes.size();
    for (int i = 0; i < thread_count; i++) {
        sum += results[i];
    }

    auto end = chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start_time;
    cout << sum << " primes found after " << duration.count() << "s." << endl;
}
