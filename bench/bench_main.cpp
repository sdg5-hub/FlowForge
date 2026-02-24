#include <iomanip>
#include <iostream>
#include <chrono>
#include <vector>

#include "flowforge/sim/Simulation.h"

namespace {

void RunCase(std::size_t n) {
  flowforge::SimConfig cfg;
  cfg.agent_count = n;
  cfg.seed = 123;

  flowforge::Simulation sim(cfg);
  sim.Reset(flowforge::DemoMode::kCityRushHour);

  constexpr int kWarmup = 30;
  constexpr int kFrames = 180;

  for (int i = 0; i < kWarmup; ++i) {
    sim.Step();
  }

  std::uint64_t frame_ns = 0;
  std::uint64_t neigh_ns = 0;
  for (int i = 0; i < kFrames; ++i) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    sim.Step();
    const auto t1 = std::chrono::high_resolution_clock::now();
    frame_ns += static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    neigh_ns += sim.neighbor_query_ns();
  }

  const double ms = static_cast<double>(frame_ns) / kFrames / 1e6;
  const double nq_ms = static_cast<double>(neigh_ns) / kFrames / 1e6;
  std::cout << std::setw(6) << n << " agents | " << std::fixed << std::setprecision(3)
            << ms << " ms/frame | " << nq_ms << " ms neighbor queries\n";
}

}  // namespace

int main() {
  std::cout << "FlowForge Benchmark\n";
  std::cout << "===================\n";
  for (std::size_t n : std::vector<std::size_t>{1000, 5000, 10000, 20000}) {
    RunCase(n);
  }
  return 0;
}
