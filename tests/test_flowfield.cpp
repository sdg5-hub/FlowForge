#include <cmath>

#include "flowforge/sim/FlowField.h"
#include "flowforge/sim/Grid.h"

TEST(FlowFieldGenerationProducesFiniteDirections) {
  flowforge::Grid g(220, 220, 1.0f);
  g.BuildCityRushHour();
  CHECK(!g.destinations().empty());

  flowforge::FlowFieldSystem ff(g);
  ff.RebuildAll();

  int checked = 0;
  for (int y = 0; y < g.h() && checked < 64; y += 7) {
    for (int x = 0; x < g.w() && checked < 64; x += 7) {
      if (!g.IsWalkable(x, y)) continue;
      const flowforge::Vec2 d = ff.Direction(0, g.CellCenter(x, y));
      CHECK(std::isfinite(d.x));
      CHECK(std::isfinite(d.y));
      CHECK(d.LengthSq() <= 1.01f);
      ++checked;
    }
  }
  CHECK(checked > 0);
}
