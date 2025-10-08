#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <chrono>
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
        for(size_t i{current_index+1}; i < nums.size(); ++i)
        {
            if(nums.at(i) % current_prime == 0)
                nums.at(i) = -1;
        }
        do
            current_prime = nums.at(++current_index);
        while(current_prime == -1 && current_index < nums.size());
    }
    auto elapsed{chrono::steady_clock::now() - start_time};

    //copy_if(nums.begin(), nums.end(),
    //        ostream_iterator<int>{cout, " "}, [](int i){return i != -1;});
    cout << "\nElapsed time: " << chrono::duration_cast<chrono::milliseconds>(elapsed).count() << " ms." << endl;
}
