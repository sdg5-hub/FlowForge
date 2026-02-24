#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace {
using TestFn = std::function<void()>;
std::vector<std::pair<std::string, TestFn>>& Registry() {
  static std::vector<std::pair<std::string, TestFn>> tests;
  return tests;
}

struct Registrar {
  Registrar(const std::string& name, TestFn fn) { Registry().push_back({name, fn}); }
};
}  // namespace

#define TEST(name)                                  \
  void name();                                      \
  static Registrar reg_##name(#name, name);        \
  void name()

#define CHECK(cond)                                                               \
  do {                                                                            \
    if (!(cond)) {                                                                \
      throw std::runtime_error(std::string("CHECK failed: ") + #cond);          \
    }                                                                             \
  } while (0)

#include "test_astar.cpp"
#include "test_flowfield.cpp"
#include "test_spatial_grid.cpp"

int main() {
  int fail = 0;
  for (const auto& [name, fn] : Registry()) {
    try {
      fn();
      std::cout << "[PASS] " << name << "\n";
    } catch (const std::exception& e) {
      ++fail;
      std::cout << "[FAIL] " << name << " -> " << e.what() << "\n";
    }
  }
  if (fail) {
    std::cout << fail << " test(s) failed\n";
  }
  return fail == 0 ? 0 : 1;
}
