#include "flowforge/sim/Simulation.h"

#include <chrono>
#include <cmath>

#include "flowforge/util/Profiler.h"

namespace flowforge {

Simulation::Simulation(const SimConfig& cfg)
    : cfg_(cfg),
      grid_(cfg.world_w, cfg.world_h, cfg.cell_size),
      spatial_(cfg.world_w, cfg.world_h, cfg.cell_size),
      flow_(grid_),
      metrics_(grid_),
      rng_(cfg.seed) {
  Reset(DemoMode::kCityRushHour);
}

void Simulation::Reset(DemoMode mode) {
  demo_mode_ = mode;
  sim_time_ = 0.0f;
  tick_ = 0;
  frame_ns_ = 0;
  neighbor_query_ns_ = 0;

  if (mode == DemoMode::kEvacuation) {
    grid_.BuildEvacuation();
  } else {
    grid_.BuildCityRushHour();
  }

  agents_.Spawn(grid_, cfg_, rng_, mode);
  flow_.RebuildAll();
  metrics_.Update(agents_, sim_time_, mode == DemoMode::kEvacuation);
}

void Simulation::SetObstacleBrush(int x0, int y0, int x1, int y1, bool obstacle) {
  grid_.PaintObstacleRect(x0, y0, x1, y1, obstacle);
  map_dirty_ = true;
}

void Simulation::Step() {
  FF_ZONE();
  const auto t0 = std::chrono::high_resolution_clock::now();

  if (map_dirty_) {
    flow_.RebuildAll();
    map_dirty_ = false;
  }

  UpdateLights();
  StepAgents();
  sim_time_ += cfg_.fixed_dt;
  ++tick_;
  metrics_.Update(agents_, sim_time_, demo_mode_ == DemoMode::kEvacuation);
  const auto t1 = std::chrono::high_resolution_clock::now();
  frame_ns_ = static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
}

void Simulation::UpdateLights() {
  for (auto& l : grid_.mutable_lights()) {
    l.Step(cfg_.fixed_dt);
  }
}

Vec2 Simulation::DesiredDirection(std::size_t i) {
  const Vec2 pos{agents_.px[i], agents_.py[i]};

  if (path_mode_ == PathMode::kFlowField) {
    return flow_.Direction(agents_.destination[i], pos);
  }

  // A* debug mode: compute a short immediate path each frame.
  int sx = 0;
  int sy = 0;
  if (!grid_.WorldToCell(pos, &sx, &sy)) return {0.0f, 0.0f};

  const Vec2 dst = grid_.destinations()[agents_.destination[i]];
  int gx = 0;
  int gy = 0;
  if (!grid_.WorldToCell(dst, &gx, &gy)) return {0.0f, 0.0f};

  std::vector<Vec2> path;
  if (!AStar::FindPath(grid_, sx, sy, gx, gy, &path) || path.size() < 2) {
    return {0.0f, 0.0f};
  }
  return (path[1] - pos).Normalized();
}

bool Simulation::ShouldStopAtRed(std::size_t i, int cx, int cy, const Vec2& dir) const {
  if (static_cast<AgentType>(agents_.type[i]) != AgentType::kVehicle) return false;

  for (const auto& light : grid_.lights()) {
    if (std::abs(light.x - cx) > 2 || std::abs(light.y - cy) > 2) continue;

    const bool going_vertical = std::abs(dir.y) > std::abs(dir.x);
    if (going_vertical && !light.VerticalGreen()) return true;
    if (!going_vertical && light.VerticalGreen()) return true;
  }
  return false;
}

void Simulation::StepAgents() {
  spatial_.reset_query_ns();
  spatial_.Rebuild(agents_);

  std::vector<std::size_t> neighbors;
  neighbors.reserve(128);

  for (std::size_t i = 0; i < agents_.size(); ++i) {
    if (!agents_.active[i]) continue;

    const AgentType t = static_cast<AgentType>(agents_.type[i]);
    const AgentParams params = (t == AgentType::kVehicle) ? cfg_.vehicle : cfg_.pedestrian;

    const Vec2 pos{agents_.px[i], agents_.py[i]};
    Vec2 vel{agents_.vx[i], agents_.vy[i]};

    Vec2 desired = DesiredDirection(i);
    Vec2 separation{0.0f, 0.0f};

    const bool full_sep =
        (t == AgentType::kVehicle) || (((static_cast<std::uint64_t>(i) + tick_) & 1ULL) == 0ULL);
    if (full_sep) {
      spatial_.QueryNearby(pos.x, pos.y, &neighbors);
      const float sep_r2 = cfg_.separation_radius * cfg_.separation_radius;
      for (std::size_t n : neighbors) {
        if (n == i || !agents_.active[n]) continue;
        const Vec2 diff{pos.x - agents_.px[n], pos.y - agents_.py[n]};
        const float d2 = diff.LengthSq();
        if (d2 < 1e-6f || d2 > sep_r2) continue;
        const float w = (sep_r2 - d2) / sep_r2;
        separation += diff.Normalized() * (w * cfg_.separation_strength);
      }
    }

    int cx = 0;
    int cy = 0;
    if (grid_.WorldToCell(pos, &cx, &cy) && ShouldStopAtRed(i, cx, cy, desired)) {
      desired = {0.0f, 0.0f};
    }

    const Vec2 desired_v = desired * params.max_speed;
    Vec2 accel = (desired_v - vel) * params.turn_rate + separation;

    const float accel_len = accel.Length();
    if (accel_len > params.max_accel) {
      accel = accel * (params.max_accel / accel_len);
    }

    vel += accel * cfg_.fixed_dt;

    const float vlen = vel.Length();
    if (vlen > params.max_speed) {
      vel = vel * (params.max_speed / vlen);
    }

    Vec2 np = grid_.ClampWorld(pos + vel * cfg_.fixed_dt);
    int ncx = 0;
    int ncy = 0;
    if (!grid_.WorldToCell(np, &ncx, &ncy) || !grid_.IsWalkable(ncx, ncy)) {
      np = pos;
      vel = vel * 0.1f;
    }

    agents_.px[i] = np.x;
    agents_.py[i] = np.y;
    agents_.vx[i] = vel.x;
    agents_.vy[i] = vel.y;
    agents_.speed[i] = vel.Length();
    if (agents_.speed[i] > 1e-3f) {
      agents_.heading[i] = std::atan2(vel.y, vel.x);
    }

    const Vec2 dst = grid_.destinations()[agents_.destination[i]];
    const Vec2 to_dst = dst - np;
    if (to_dst.LengthSq() <= cfg_.arrive_radius * cfg_.arrive_radius) {
      if (demo_mode_ == DemoMode::kEvacuation) {
        agents_.active[i] = 0;
      } else {
        agents_.destination[i] = static_cast<std::uint16_t>(
            rng_.RangeInt(0, static_cast<int>(grid_.destinations().size() - 1)));
      }
    }
  }

  neighbor_query_ns_ = spatial_.last_query_ns();
}

}  // namespace flowforge
