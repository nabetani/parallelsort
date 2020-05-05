#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <parallel/algorithm>
#include <random>
#include <string>
#include <vector>

struct random_uint32 {
  std::mt19937_64 gen_{9915530857934838174ULL};
  std::uniform_int_distribution<std::uint32_t> dist_{0, ~0U};
  uint32_t operator()() { return dist_(gen_); }
};

struct gnu_p_sort {
  template <typename itorator>               //
  static void sort(itorator a, itorator b) { //
    __gnu_parallel::sort(a, b);
  }
  template <typename itorator>                      //
  static void stable_sort(itorator a, itorator b) { //
    __gnu_parallel::stable_sort(a, b);
  }
  static std::string name() { return "gnu_parallel"; }
};

struct std_sort {
  template <typename itorator>               //
  static void sort(itorator a, itorator b) { //
    std::sort(a, b);
  }
  template <typename itorator>                      //
  static void stable_sort(itorator a, itorator b) { //
    std::stable_sort(a, b);
  }
  static std::string name() { return "std"; }
};

template <typename value_type, //
          typename sorter,
          typename generator> //
void test() {
  size_t min_size = 1 << 8;
  size_t max_size = 1 << 16;
  using clock = std::chrono::high_resolution_clock;
  for (size_t size = min_size; size <= max_size; size <<= 1) {
    generator g;
    std::vector<value_type> v0(size);
    using itor_t = typename std::vector<value_type>::iterator;
    for (auto &e : v0) {
      e = g();
    }
    auto proc = [&](std::function<void(itor_t, itor_t)> s) -> double {
      auto v = v0;
      using namespace std::chrono;
      auto start = clock::now();
      s(begin(v), end(v));
      auto dur = clock::now() - start;
      return duration_cast<nanoseconds>(dur).count() * 1e-6;
    };
    auto sort_tick = proc([](itor_t a, itor_t b) { sorter::sort(a, b); });
    auto stable_sort_tick =
        proc([](itor_t a, itor_t b) { sorter::stable_sort(a, b); });
    std::cout                     //
        << sorter::name() << ", " //
        << size << ", "           //
        << sort_tick << ", "      //
        << stable_sort_tick << std::endl;
  }
}

int main() {
  test<uint32_t, gnu_p_sort, random_uint32>();
  test<uint32_t, std_sort, random_uint32>();
  // test<std::string, random_string>();
}