#pragma once

#include <vector>

#include "flowforge/sim/Grid.h"
#include "flowforge/util/Types.h"

namespace flowforge {

class AStar {
 public:
  static bool FindPath(const Grid& grid, int sx, int sy, int gx, int gy,
                       std::vector<Vec2>* out_world_path);
};

}  // namespace flowforge
