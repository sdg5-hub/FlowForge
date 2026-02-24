#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "flowforge/sim/AStar.h"
#include "flowforge/sim/AgentSoA.h"
#include "flowforge/sim/Config.h"
#include "flowforge/sim/FlowField.h"
#include "flowforge/sim/Grid.h"
#include "flowforge/sim/Metrics.h"
#include "flowforge/sim/SpatialGrid.h"
#include "flowforge/util/Rng.h"
#include "flowforge/util/Types.h"

namespace flowforge {

struct OverlayState {
  bool heatmap = true;
  bool flow_vectors = false;
  bool collision_debug = false;
  bool agent_ids = false;
};

class Simulation {
 public:
  explicit Simulation(const SimConfig& cfg);

  void Reset(DemoMode mode);
  void Step();

  void SetPathMode(PathMode mode) { path_mode_ = mode; }
  PathMode path_mode() const { return path_mode_; }

  const SimConfig& cfg() const { return cfg_; }
  const Grid& grid() const { return grid_; }
  const AgentSoA& agents() const { return agents_; }
  const MetricsSnapshot& metrics() const { return metrics_.snapshot(); }

  float sim_time() const { return sim_time_; }
  DemoMode demo_mode() const { return demo_mode_; }

  OverlayState& overlays() { return overlays_; }
  const OverlayState& overlays() const { return overlays_; }

  void SetObstacleBrush(int x0, int y0, int x1, int y1, bool obstacle);

  std::uint64_t frame_ns() const { return frame_ns_; }
  std::uint64_t neighbor_query_ns() const { return neighbor_query_ns_; }

 private:
  SimConfig cfg_;
  Grid grid_;
  AgentSoA agents_;
  SpatialGrid spatial_;
  FlowFieldSystem flow_;
  Metrics metrics_;
  Rng rng_;

  DemoMode demo_mode_{DemoMode::kCityRushHour};
  PathMode path_mode_{PathMode::kFlowField};
  OverlayState overlays_{};

  float sim_time_{0.0f};
  std::uint64_t frame_ns_{0};
  std::uint64_t neighbor_query_ns_{0};
  bool map_dirty_{false};
  std::uint64_t tick_{0};

  void UpdateLights();
  void StepAgents();
  Vec2 DesiredDirection(std::size_t i);
  bool ShouldStopAtRed(std::size_t i, int cx, int cy, const Vec2& dir) const;
};

}  // namespace flowforge
