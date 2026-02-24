#include "flowforge/sim/AStar.h"
#include "flowforge/sim/Grid.h"

TEST(AStarFindsPathThroughGap) {
  flowforge::Grid g(12, 8, 1.0f);
  for (int y = 0; y < g.h(); ++y) {
    for (int x = 0; x < g.w(); ++x) {
      g.Set(x, y, flowforge::CellType::kRoad);
    }
  }
  for (int y = 0; y < g.h(); ++y) {
    g.Set(5, y, flowforge::CellType::kObstacle);
  }
  g.Set(5, 4, flowforge::CellType::kRoad);

  std::vector<flowforge::Vec2> path;
  const bool ok = flowforge::AStar::FindPath(g, 1, 1, 10, 6, &path);
  CHECK(ok);
  CHECK(path.size() >= 2);
}
