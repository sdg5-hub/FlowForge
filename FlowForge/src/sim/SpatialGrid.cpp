#include "flowforge/sim/SpatialGrid.h"

#include <chrono>

namespace flowforge {

SpatialGrid::SpatialGrid(int world_w, int world_h, float cell_size)
    : w_(world_w), h_(world_h), cell_size_(cell_size), buckets_(static_cast<std::size_t>(world_w * world_h)) {}

void SpatialGrid::Rebuild(const AgentSoA& agents) {
  for (auto& b : buckets_) {
    b.clear();
  }

  for (std::size_t i = 0; i < agents.size(); ++i) {
    if (!agents.active[i]) continue;
    const int cx = static_cast<int>(agents.px[i] / cell_size_);
    const int cy = static_cast<int>(agents.py[i] / cell_size_);
    if (cx < 0 || cx >= w_ || cy < 0 || cy >= h_) continue;
    buckets_[static_cast<std::size_t>(cy * w_ + cx)].push_back(i);
  }
}

void SpatialGrid::QueryNearby(float x, float y, std::vector<std::size_t>* out_indices) const {
  const auto t0 = std::chrono::high_resolution_clock::now();
  out_indices->clear();

  const int cx = static_cast<int>(x / cell_size_);
  const int cy = static_cast<int>(y / cell_size_);

  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      const int nx = cx + dx;
      const int ny = cy + dy;
      if (nx < 0 || nx >= w_ || ny < 0 || ny >= h_) continue;
      const auto& b = buckets_[static_cast<std::size_t>(ny * w_ + nx)];
      out_indices->insert(out_indices->end(), b.begin(), b.end());
    }
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  last_query_ns_ += static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
}

}  // namespace flowforge
