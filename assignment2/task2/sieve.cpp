#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
using namespace std;

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

    istringstream ss{argv[2]};
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
    cout << "Max: " << max << endl;
    while(current_prime*current_prime <= max)
    {
        cout << "current_prime: " << current_prime << endl;
        for(size_t i{current_index+1}; i < nums.size(); ++i)
        {
            if(nums.at(i) % current_prime == 0)
                nums.at(i) = -1;
            else
                cout << nums.at(i) << ' ';
        }
        cout << '\n' << endl;
        do
            current_prime = nums.at(++current_index);
        while(current_prime == -1 && current_index < nums.size());
    }

    copy_if(nums.begin(), nums.end(),
            ostream_iterator<int>{cout, " "}, [](int i){return i != -1;});
    cout << endl;
}
