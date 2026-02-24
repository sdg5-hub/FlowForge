#pragma once

#include <chrono>
#include <cstdint>

#ifdef FLOWFORGE_TRACY
#include <tracy/Tracy.hpp>
#define FF_ZONE() ZoneScoped
#else
#define FF_ZONE()
#endif

namespace flowforge {

struct ScopedTimer {
  using Clock = std::chrono::high_resolution_clock;

  explicit ScopedTimer(std::uint64_t* out_ns) : out_ns_(out_ns), start_(Clock::now()) {}
  ~ScopedTimer() {
    if (out_ns_ != nullptr) {
      *out_ns_ += static_cast<std::uint64_t>(
          std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_).count());
    }
  }

 private:
  std::uint64_t* out_ns_;
  Clock::time_point start_;
};

}  // namespace flowforge
