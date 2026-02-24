#pragma once

#include <cstdint>

namespace flowforge {

class Rng {
 public:
  explicit Rng(std::uint64_t seed = 0xC0FFEEULL) : state_(seed ? seed : 0xC0FFEEULL) {}

  std::uint32_t NextU32() {
    // xorshift64*
    state_ ^= state_ >> 12;
    state_ ^= state_ << 25;
    state_ ^= state_ >> 27;
    return static_cast<std::uint32_t>((state_ * 2685821657736338717ULL) >> 32);
  }

  float NextFloat01() { return static_cast<float>(NextU32()) / 4294967295.0f; }

  int RangeInt(int lo, int hi) {
    const std::uint32_t span = static_cast<std::uint32_t>(hi - lo + 1);
    return lo + static_cast<int>(NextU32() % span);
  }

 private:
  std::uint64_t state_;
};

}  // namespace flowforge
