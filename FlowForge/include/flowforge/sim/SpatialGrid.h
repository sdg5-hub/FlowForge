#pragma once

#include <cstddef>
#include <vector>

#include "flowforge/sim/AgentSoA.h"

namespace flowforge {

class SpatialGrid {
 public:
  SpatialGrid(int world_w, int world_h, float cell_size);

  void Rebuild(const AgentSoA& agents);
  void QueryNearby(float x, float y, std::vector<std::size_t>* out_indices) const;

  std::uint64_t last_query_ns() const { return last_query_ns_; }
  void reset_query_ns() { last_query_ns_ = 0; }

 private:
  int w_;
  int h_;
  float cell_size_;
  std::vector<std::vector<std::size_t>> buckets_;
  mutable std::uint64_t last_query_ns_{0};
};

}  // namespace flowforge
