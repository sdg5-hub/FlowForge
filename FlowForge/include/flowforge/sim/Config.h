#pragma once

#include <cstddef>
#include <cstdint>

namespace flowforge {

struct AgentParams {
  float max_speed = 20.0f;
  float max_accel = 60.0f;
  float turn_rate = 8.0f;
  float radius = 2.2f;
};

struct SimConfig {
  int world_w = 220;
  int world_h = 220;
  float cell_size = 6.0f;
  std::size_t agent_count = 10000;
  float fixed_dt = 1.0f / 60.0f;
  float separation_radius = 6.5f;
  float separation_strength = 55.0f;
  float arrive_radius = 5.0f;
  std::uint64_t seed = 42;
  AgentParams pedestrian{12.0f, 55.0f, 10.0f, 1.8f};
  AgentParams vehicle{24.0f, 45.0f, 3.5f, 2.8f};
};

}  // namespace flowforge
