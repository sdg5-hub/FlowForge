#pragma once

#include <vector>

#include "flowforge/sim/Grid.h"
#include "flowforge/util/Types.h"

namespace flowforge {

class FlowFieldSystem {
 public:
  explicit FlowFieldSystem(const Grid& grid) : grid_(grid) {}

  void RebuildAll();
  Vec2 Direction(std::size_t destination_index, const Vec2& world_pos) const;

 private:
  struct Field {
    std::vector<float> cost;
    std::vector<Vec2> dir;
  };

  const Grid& grid_;
  std::vector<Field> fields_;

  void BuildSingle(const Vec2& destination, Field* out);
};

}  // namespace flowforge
