#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include "flowforge/sim/AgentSoA.h"
#include "flowforge/sim/Grid.h"

namespace flowforge {

struct Hotspot {
  int x = 0;
  int y = 0;
  int count = 0;
};

struct MetricsSnapshot {
  float avg_speed = 0.0f;
  float congestion = 0.0f;
  float evac_time = 0.0f;
  int active_agents = 0;
  std::array<Hotspot, 3> hotspots{};
  std::vector<float> density;
};

class Metrics {
 public:
  explicit Metrics(const Grid& grid);

  void Update(const AgentSoA& agents, float sim_time, bool evacuation_mode);
  const MetricsSnapshot& snapshot() const { return snap_; }

  std::string HudText(float fps, int total_agents, float speed_mul, bool paused) const;

 private:
  const Grid& grid_;
  MetricsSnapshot snap_;
};

}  // namespace flowforge
