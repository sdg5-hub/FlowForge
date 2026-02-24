#include "flowforge/sim/Metrics.h"

#include <algorithm>
#include <array>
#include <sstream>

namespace flowforge {

Metrics::Metrics(const Grid& grid) : grid_(grid) {
  snap_.density.resize(static_cast<std::size_t>(grid_.w() * grid_.h()), 0.0f);
}

void Metrics::Update(const AgentSoA& agents, float sim_time, bool evacuation_mode) {
  std::fill(snap_.density.begin(), snap_.density.end(), 0.0f);

  float speed_sum = 0.0f;
  int slow = 0;
  int active = 0;

  for (std::size_t i = 0; i < agents.size(); ++i) {
    if (!agents.active[i]) continue;
    ++active;
    speed_sum += agents.speed[i];
    if (agents.speed[i] < 4.0f) ++slow;

    const int cx = static_cast<int>(agents.px[i] / grid_.cell_size());
    const int cy = static_cast<int>(agents.py[i] / grid_.cell_size());
    if (grid_.InBounds(cx, cy)) {
      snap_.density[static_cast<std::size_t>(grid_.Index(cx, cy))] += 1.0f;
    }
  }

  snap_.active_agents = active;
  snap_.avg_speed = active > 0 ? speed_sum / static_cast<float>(active) : 0.0f;
  snap_.congestion = active > 0 ? static_cast<float>(slow) / static_cast<float>(active) : 0.0f;

  std::array<Hotspot, 3> tops{};
  for (int y = 0; y < grid_.h(); ++y) {
    for (int x = 0; x < grid_.w(); ++x) {
      const int count = static_cast<int>(snap_.density[static_cast<std::size_t>(grid_.Index(x, y))]);
      if (count <= tops[2].count) continue;
      tops[2] = {x, y, count};
      if (tops[2].count > tops[1].count) std::swap(tops[2], tops[1]);
      if (tops[1].count > tops[0].count) std::swap(tops[1], tops[0]);
    }
  }
  snap_.hotspots = tops;

  if (evacuation_mode) {
    if (snap_.active_agents == 0 && snap_.evac_time <= 0.0f) {
      snap_.evac_time = sim_time;
    }
  } else {
    snap_.evac_time = 0.0f;
  }
}

std::string Metrics::HudText(float fps, int total_agents, float speed_mul, bool paused) const {
  std::ostringstream os;
  os.setf(std::ios::fixed);
  os.precision(1);
  os << "FPS " << fps << " | Agents " << snap_.active_agents << "/" << total_agents
     << " | Speed x" << speed_mul << " | Avg " << snap_.avg_speed
     << " | Cong " << (snap_.congestion * 100.0f) << "%";
  if (snap_.evac_time > 0.0f) {
    os << " | Evac " << snap_.evac_time << "s";
  }
  if (paused) {
    os << " | PAUSED";
  }
  return os.str();
}

}  // namespace flowforge
