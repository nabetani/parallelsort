#include <algorithm>
#include <chrono>
#include <cstdint>
#include <execution>
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

struct par {
  static constexpr std::execution::parallel_policy e = std::execution::par;
  static std::string name() { return "std(par)"; }
};

struct seq {
  static constexpr std::execution::sequenced_policy e = std::execution::seq;
  static std::string name() { return "std(seq)"; }
};

struct par_unseq {
  static constexpr std::execution::parallel_unsequenced_policy e =
      std::execution::par_unseq;
  static std::string name() { return "std(par_unseq)"; }
};

struct unseq {
  static constexpr std::execution::unsequenced_policy e = std::execution::unseq;
  static std::string name() { return "std(unseq)"; }
};

template <typename exec> //
struct std_sort_exec {
  template <typename itorator>               //
  static void sort(itorator a, itorator b) { //
    std::sort(exec::e, a, b);
  }
  template <typename itorator>                      //
  static void stable_sort(itorator a, itorator b) { //
    std::stable_sort(exec::e, a, b);
  }
  static std::string name() { return exec::name(); }
};

template <typename value_type, //
          typename sorter,
          typename generator> //
void test(std::ostream &stream) {
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
    stream << '"' << sorter::name() << '"' << ", "    //
           << '"' << generator::name() << '"' << ", " //
           << size << ", "                            //
           << sort_tick << ", "                       //
           << stable_sort_tick << std::endl;
  }
}

template <typename sorter> void run_tests(std::ostream &stream) {
  test<uint32_t, sorter, random_uint32>(stream);
  test<uint32_t, sorter, few_uint32<8>>(stream);
  test<std::string, sorter, random_string<>>(stream);
  test<std::string, sorter, few_strings<8>>(stream);
}

void test_all(std::ostream &stream) {
  run_tests<gnu_p_sort>(stream);
  run_tests<std_sort>(stream);
  // run_tests<std_sort_exec<seq>>(stream); // C++17(single-thread, no-SIMD)
  run_tests<std_sort_exec<par>>(stream);       // C++17(muti-thread)
  run_tests<std_sort_exec<par_unseq>>(stream); // C++17(muti-thread and/or SIMD)
  run_tests<std_sort_exec<unseq>>(stream);     // C++20(SIMD)
}

int main() {
  test_all(std::cerr); // 練習
  test_all(std::cout); // 本番
}