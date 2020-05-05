#include <algorithm>
#include <iostream>
#include <parallel/algorithm>
#include <vector>

int main() {
  std::vector<int> m{3, 1, 4, 1, 5, 9, 2};
  __gnu_parallel::sort(begin(m), end(m));
  for (auto e : m) {
    std::cout << e;
  }
  std::cout << std::endl;
}