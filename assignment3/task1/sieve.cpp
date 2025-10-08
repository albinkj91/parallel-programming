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

void sieve_chunk(int32_t const start, int32_t const end, vector<int32_t> const& primes, vector<int32_t> & nums)
{
    for(auto prime : primes)
    {
        for(int32_t j{start}; j < end; ++j)
        {
            if(nums.at(j) % prime == 0)
                nums.at(j) = -1;
        }
    }
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
            if(nums.at(i) % current_prime == 0)
                nums.at(i) = -1;
        }
        do
            current_prime = nums.at(++current_index);
        while(current_prime == -1 && current_index < nums.size());
    }

    int32_t parallel_index{current_prime};
    int32_t chunk_size{(max - parallel_index) / thread_count};
    int32_t remainder{(max - parallel_index) % thread_count};
    vector<thread> threads{};

    threads.push_back(thread{
            sieve_chunk,
            parallel_index - 1,
            parallel_index + chunk_size + remainder - 1,
            ref(primes),
            ref(nums)});

    parallel_index += remainder;
    for(int i{1}; i < thread_count; ++i)
    {
        parallel_index += chunk_size;
        threads.push_back(thread{
                sieve_chunk,
                parallel_index - 1,
                parallel_index+chunk_size - 1,
                ref(primes),
                ref(nums)});
    }

    for(auto& thread : threads)
        thread.join();

    auto elapsed{chrono::steady_clock::now() - start_time};
    //copy_if(nums.begin(), nums.end(),
    //        ostream_iterator<int>{cout, " "}, [](int i){return i != -1;});
    cout << "\nElapsed time: " << chrono::duration_cast<chrono::milliseconds>(elapsed).count() << " ms." << endl;
}
