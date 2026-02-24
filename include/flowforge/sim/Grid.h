#pragma once

#include <vector>

#include "flowforge/util/Types.h"

namespace flowforge {

struct TrafficLight {
  int x = 0;
  int y = 0;
  float cycle_seconds = 8.0f;
  float green_vertical = 4.0f;
  float phase = 0.0f;

  bool VerticalGreen() const;
  void Step(float dt);
};

class Grid {
 public:
  Grid(int w, int h, float cell_size);

  void BuildCityRushHour();
  void BuildEvacuation();

  bool InBounds(int x, int y) const;
  int Index(int x, int y) const;

  CellType Get(int x, int y) const;
  void Set(int x, int y, CellType type);

  bool IsWalkable(int x, int y) const;
  bool IsObstacle(int x, int y) const;

  Vec2 CellCenter(int x, int y) const;
  Vec2 ClampWorld(const Vec2& p) const;

  bool WorldToCell(const Vec2& p, int* out_x, int* out_y) const;

  int w() const { return w_; }
  int h() const { return h_; }
  float cell_size() const { return cell_size_; }

  const std::vector<Vec2>& destinations() const { return destinations_; }
  const std::vector<TrafficLight>& lights() const { return lights_; }
  std::vector<TrafficLight>& mutable_lights() { return lights_; }

  void PaintObstacleRect(int x0, int y0, int x1, int y1, bool obstacle);

 private:
  int w_;
  int h_;
  float cell_size_;
  std::vector<CellType> cells_;
  std::vector<Vec2> destinations_;
  std::vector<TrafficLight> lights_;
};

}  // namespace flowforge
