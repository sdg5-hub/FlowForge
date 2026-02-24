#include "flowforge/sim/AgentSoA.h"

namespace flowforge {

void AgentSoA::Resize(std::size_t n) {
  px.resize(n);
  py.resize(n);
  vx.resize(n);
  vy.resize(n);
  heading.resize(n);
  speed.resize(n);
  type.resize(n);
  destination.resize(n);
  active.resize(n);
  id.resize(n);
}

void AgentSoA::Spawn(const Grid& grid, const SimConfig& cfg, Rng& rng, DemoMode mode) {
  Resize(cfg.agent_count);
  const std::size_t dest_count = grid.destinations().size();

  for (std::size_t i = 0; i < cfg.agent_count; ++i) {
    int cx = 0;
    int cy = 0;
    do {
      cx = rng.RangeInt(0, grid.w() - 1);
      cy = rng.RangeInt(0, grid.h() - 1);
    } while (!grid.IsWalkable(cx, cy));

    const Vec2 spawn = grid.CellCenter(cx, cy);
    px[i] = spawn.x;
    py[i] = spawn.y;
    vx[i] = 0.0f;
    vy[i] = 0.0f;
    heading[i] = 0.0f;
    speed[i] = 0.0f;

    const bool vehicle_heavy = (mode == DemoMode::kCityRushHour);
    const int vehicle_ratio = vehicle_heavy ? 45 : 20;
    type[i] = static_cast<std::uint8_t>(rng.RangeInt(0, 99) < vehicle_ratio ? AgentType::kVehicle
                                                                             : AgentType::kPedestrian);
    destination[i] = static_cast<std::uint16_t>(dest_count > 0 ? rng.RangeInt(0, static_cast<int>(dest_count - 1))
                                                                : 0);
    active[i] = 1;
    id[i] = static_cast<std::uint32_t>(i);
  }
}

}  // namespace flowforge
