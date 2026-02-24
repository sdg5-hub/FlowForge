#include "flowforge/sim/FlowField.h"

#include <cmath>
#include <limits>
#include <queue>

namespace flowforge {

void FlowFieldSystem::RebuildAll() {
  fields_.clear();
  fields_.resize(grid_.destinations().size());
  for (std::size_t i = 0; i < grid_.destinations().size(); ++i) {
    BuildSingle(grid_.destinations()[i], &fields_[i]);
  }
}

Vec2 FlowFieldSystem::Direction(std::size_t destination_index, const Vec2& world_pos) const {
  if (destination_index >= fields_.size()) return {0.0f, 0.0f};

  int x = 0;
  int y = 0;
  if (!grid_.WorldToCell(world_pos, &x, &y)) return {0.0f, 0.0f};

  return fields_[destination_index].dir[static_cast<std::size_t>(grid_.Index(x, y))];
}

void FlowFieldSystem::BuildSingle(const Vec2& destination, Field* out) {
  const int n = grid_.w() * grid_.h();
  out->cost.assign(static_cast<std::size_t>(n), std::numeric_limits<float>::infinity());
  out->dir.assign(static_cast<std::size_t>(n), {0.0f, 0.0f});

  int dx = 0;
  int dy = 0;
  if (!grid_.WorldToCell(destination, &dx, &dy)) return;

  std::queue<std::pair<int, int>> q;
  out->cost[static_cast<std::size_t>(grid_.Index(dx, dy))] = 0.0f;
  q.push({dx, dy});

  constexpr int kDx[4] = {1, -1, 0, 0};
  constexpr int kDy[4] = {0, 0, 1, -1};

  while (!q.empty()) {
    auto [cx, cy] = q.front();
    q.pop();
    const int ci = grid_.Index(cx, cy);
    const float cc = out->cost[static_cast<std::size_t>(ci)];

    for (int i = 0; i < 4; ++i) {
      const int nx = cx + kDx[i];
      const int ny = cy + kDy[i];
      if (!grid_.InBounds(nx, ny) || !grid_.IsWalkable(nx, ny)) continue;
      const int ni = grid_.Index(nx, ny);
      const float nc = cc + 1.0f;
      if (nc < out->cost[static_cast<std::size_t>(ni)]) {
        out->cost[static_cast<std::size_t>(ni)] = nc;
        q.push({nx, ny});
      }
    }
  }

  for (int y = 0; y < grid_.h(); ++y) {
    for (int x = 0; x < grid_.w(); ++x) {
      if (!grid_.IsWalkable(x, y)) continue;

      const int ci = grid_.Index(x, y);
      const float base = out->cost[static_cast<std::size_t>(ci)];
      if (!std::isfinite(base) || base <= 0.0f) continue;

      float best = base;
      Vec2 dir{0.0f, 0.0f};
      for (int i = 0; i < 4; ++i) {
        const int nx = x + kDx[i];
        const int ny = y + kDy[i];
        if (!grid_.InBounds(nx, ny) || !grid_.IsWalkable(nx, ny)) continue;
        const float c = out->cost[static_cast<std::size_t>(grid_.Index(nx, ny))];
        if (c < best) {
          best = c;
          dir = (grid_.CellCenter(nx, ny) - grid_.CellCenter(x, y)).Normalized();
        }
      }
      out->dir[static_cast<std::size_t>(ci)] = dir;
    }
  }
}

}  // namespace flowforge
