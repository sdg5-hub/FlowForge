#include "flowforge/sim/AgentSoA.h"
#include "flowforge/sim/SpatialGrid.h"

TEST(SpatialGridReturnsNearbyAgents) {
  flowforge::AgentSoA a;
  a.Resize(3);
  a.px = {10.0f, 11.0f, 90.0f};
  a.py = {10.0f, 12.0f, 90.0f};
  a.active = {1, 1, 1};

  flowforge::SpatialGrid grid(100, 100, 4.0f);
  grid.Rebuild(a);

  std::vector<std::size_t> n;
  grid.QueryNearby(10.0f, 10.0f, &n);
  CHECK(n.size() >= 2);
}
