#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <parallel/algorithm>
#include <random>
#include <sstream>
#include <string>
#include <vector>

struct random_uint32 {
  std::mt19937_64 gen_{9915530857934838174ULL};
  std::uniform_int_distribution<std::uint32_t> dist_{0, ~0U};
  uint32_t operator()() { return dist_(gen_); }
  static std::string name() { return "random_uint32"; }
};

template <std::uint32_t kinds> //
struct few_uint32 {
  std::mt19937_64 gen_{9915530857934838174ULL};
  std::uniform_int_distribution<std::uint32_t> dist_{0, kinds};
  uint32_t operator()() { return dist_(gen_); }
  static std::string name() {
    std::stringstream ss;
    ss << "few_uint32<" << kinds << ">";
    return ss.str();
  }
};

template <size_t len = 1024> //
struct random_string {
  std::mt19937_64 gen_{9915530857934838174ULL};
  std::uniform_int_distribution<std::uint32_t> dist_{'a', 'z'};
  std::string operator()() {
    std::string r;
    r.reserve(len);
    for (size_t i = 0; i < len; ++i) {
      r += dist_(gen_);
    }
    return r;
  }
  static std::string name() {
    std::stringstream ss;
    ss << "random_string<" << len << ">";
    return ss.str();
  }
};

template <size_t kinds, size_t len = 1024> //
struct few_strings {
  std::mt19937_64 gen_{9915530857934838174ULL};
  std::uniform_int_distribution<std::uint32_t> dist_{'a', 'a' + kinds - 1};
  std::string operator()() {
    char ch = static_cast<char>(dist_(gen_));
    return std::string(len, ch);
  }
  static std::string name() {
    std::stringstream ss;
    ss << "few_strings<" << kinds << ", " << len << ">";
    return ss.str();
  }
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
  size_t min_size = 1 << 10;
  size_t max_size = 1 << 20;
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
    std::cout                                      //
        << '"' << sorter::name() << '"' << ", "    //
        << '"' << generator::name() << '"' << ", " //
        << size << ", "                            //
        << sort_tick << ", "                       //
        << stable_sort_tick << std::endl;
  }
}

int main() {
  test<uint32_t, gnu_p_sort, random_uint32>();
  test<uint32_t, std_sort, random_uint32>();
  test<uint32_t, gnu_p_sort, few_uint32<8>>();
  test<uint32_t, std_sort, few_uint32<8>>();
  test<std::string, gnu_p_sort, random_string<>>();
  test<std::string, std_sort, random_string<>>();
  test<std::string, gnu_p_sort, few_strings<8>>();
  test<std::string, std_sort, few_strings<8>>();
}