#include "flowforge/sim/AStar.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace flowforge {

namespace {
struct Node {
  int idx;
  float f;
  bool operator>(const Node& rhs) const { return f > rhs.f; }
};
}  // namespace

bool AStar::FindPath(const Grid& grid, int sx, int sy, int gx, int gy,
                     std::vector<Vec2>* out_world_path) {
  out_world_path->clear();
  if (!grid.InBounds(sx, sy) || !grid.InBounds(gx, gy)) return false;
  if (!grid.IsWalkable(sx, sy) || !grid.IsWalkable(gx, gy)) return false;

  const int n = grid.w() * grid.h();
  std::vector<float> g(static_cast<std::size_t>(n), std::numeric_limits<float>::infinity());
  std::vector<int> parent(static_cast<std::size_t>(n), -1);
  std::vector<std::uint8_t> closed(static_cast<std::size_t>(n), 0);

  auto h = [&](int x, int y) {
    return std::abs(x - gx) + std::abs(y - gy);
  };

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
  const int start = grid.Index(sx, sy);
  const int goal = grid.Index(gx, gy);
  g[static_cast<std::size_t>(start)] = 0.0f;
  open.push({start, static_cast<float>(h(sx, sy))});

  constexpr int kDx[4] = {1, -1, 0, 0};
  constexpr int kDy[4] = {0, 0, 1, -1};

  while (!open.empty()) {
    const int cur = open.top().idx;
    open.pop();
    if (closed[static_cast<std::size_t>(cur)]) continue;
    closed[static_cast<std::size_t>(cur)] = 1;

    if (cur == goal) {
      int it = cur;
      while (it != -1) {
        const int x = it % grid.w();
        const int y = it / grid.w();
        out_world_path->push_back(grid.CellCenter(x, y));
        it = parent[static_cast<std::size_t>(it)];
      }
      std::reverse(out_world_path->begin(), out_world_path->end());
      return true;
    }

    const int cx = cur % grid.w();
    const int cy = cur / grid.w();

    for (int i = 0; i < 4; ++i) {
      const int nx = cx + kDx[i];
      const int ny = cy + kDy[i];
      if (!grid.InBounds(nx, ny) || !grid.IsWalkable(nx, ny)) continue;

      const int ni = grid.Index(nx, ny);
      const float ng = g[static_cast<std::size_t>(cur)] + 1.0f;
      if (ng < g[static_cast<std::size_t>(ni)]) {
        g[static_cast<std::size_t>(ni)] = ng;
        parent[static_cast<std::size_t>(ni)] = cur;
        open.push({ni, ng + static_cast<float>(h(nx, ny))});
      }
    }
  }

  return false;
}

}  // namespace flowforge
