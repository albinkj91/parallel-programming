#include <string>
#include <vector>
using namespace std;

int main()
{
    vector<string&> test{};
    string test{"Hello"};
    test.push_back(test);
}
