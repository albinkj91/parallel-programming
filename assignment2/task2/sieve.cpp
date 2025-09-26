#include <list>
#include <iostream>
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
    if(argc < 2)
        usage(argv[0]);
    uint32_t max{static_cast<uint32_t>(stoul(argv[1]))};

    list<uint32_t> nums{};
    uint32_t start{2};
    generate_n(back_inserter(nums), max-1, [&start](){
                return start++;
            });

    auto current_prime{nums.begin()};
    uint32_t target{static_cast<uint32_t>(sqrt(max))};
    cout << "Target: " << target << endl;
    while(*current_prime <= target)
    {
        cout << "current_prime: " << *current_prime << endl;
        auto it{current_prime};
        ++it;
        for(; it != nums.end();)
        {
            if(*it % *current_prime == 0)
            {
                it = nums.erase(it);
            }
            else
            {
                cout << *it << ' ';
                ++it;
            }
        }
        cout << '\n' << endl;
        ++current_prime;
    }

    copy(nums.begin(), nums.end(),
            ostream_iterator<int>{cout, " "});
}
