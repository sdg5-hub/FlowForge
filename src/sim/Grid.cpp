#include "flowforge/sim/Grid.h"

#include <algorithm>

namespace flowforge {

bool TrafficLight::VerticalGreen() const { return phase < green_vertical; }

void TrafficLight::Step(float dt) {
  phase += dt;
  while (phase >= cycle_seconds) {
    phase -= cycle_seconds;
  }
}

Grid::Grid(int w, int h, float cell_size)
    : w_(w), h_(h), cell_size_(cell_size), cells_(static_cast<std::size_t>(w * h), CellType::kObstacle) {}

bool Grid::InBounds(int x, int y) const { return x >= 0 && x < w_ && y >= 0 && y < h_; }
int Grid::Index(int x, int y) const { return y * w_ + x; }

CellType Grid::Get(int x, int y) const {
  if (!InBounds(x, y)) {
    return CellType::kObstacle;
  }
  return cells_[static_cast<std::size_t>(Index(x, y))];
}

void Grid::Set(int x, int y, CellType type) {
  if (!InBounds(x, y)) {
    return;
  }
  cells_[static_cast<std::size_t>(Index(x, y))] = type;
}

bool Grid::IsWalkable(int x, int y) const {
  const CellType c = Get(x, y);
  return c == CellType::kRoad || c == CellType::kIntersection || c == CellType::kDestination;
}

bool Grid::IsObstacle(int x, int y) const { return Get(x, y) == CellType::kObstacle; }

Vec2 Grid::CellCenter(int x, int y) const { return {(x + 0.5f) * cell_size_, (y + 0.5f) * cell_size_}; }

Vec2 Grid::ClampWorld(const Vec2& p) const {
  return {Clamp(p.x, 1.0f, w_ * cell_size_ - 1.0f), Clamp(p.y, 1.0f, h_ * cell_size_ - 1.0f)};
}

bool Grid::WorldToCell(const Vec2& p, int* out_x, int* out_y) const {
  const int cx = static_cast<int>(p.x / cell_size_);
  const int cy = static_cast<int>(p.y / cell_size_);
  if (!InBounds(cx, cy)) {
    return false;
  }
  *out_x = cx;
  *out_y = cy;
  return true;
}

void Grid::BuildCityRushHour() {
  std::fill(cells_.begin(), cells_.end(), CellType::kObstacle);
  destinations_.clear();
  lights_.clear();

  auto paint_road = [&](int x0, int y0, int x1, int y1) {
    for (int y = y0; y <= y1; ++y) {
      for (int x = x0; x <= x1; ++x) {
        Set(x, y, CellType::kRoad);
      }
    }
  };

  for (int y : {20, 55, 90, 125, 160, 195}) {
    paint_road(6, y, w_ - 7, y + 8);
  }
  for (int x : {20, 55, 90, 125, 160, 195}) {
    paint_road(x, 6, x + 8, h_ - 7);
  }

  for (int y : {20, 55, 90, 125, 160, 195}) {
    for (int x : {20, 55, 90, 125, 160, 195}) {
      for (int yy = y; yy <= y + 8; ++yy) {
        for (int xx = x; xx <= x + 8; ++xx) {
          Set(xx, yy, CellType::kIntersection);
        }
      }
      lights_.push_back({x + 4, y + 4, 10.0f, 5.0f, 0.0f});
    }
  }

  std::vector<std::pair<int, int>> exits = {{24, 24},   {w_ - 25, 24}, {24, h_ - 25},
                                             {w_ - 25, h_ - 25},        {24, 94},
                                             {w_ - 25, 129}};
  for (const auto& [x, y] : exits) {
    Set(x, y, CellType::kDestination);
    destinations_.push_back(CellCenter(x, y));
  }
}

void Grid::BuildEvacuation() {
  std::fill(cells_.begin(), cells_.end(), CellType::kRoad);
  destinations_.clear();
  lights_.clear();

  // Blocky obstacles forming evacuation corridors.
  for (int y = 20; y < h_ - 20; ++y) {
    for (int x = 35; x < 50; ++x) Set(x, y, CellType::kObstacle);
    for (int x = 80; x < 96; ++x) Set(x, y, CellType::kObstacle);
    for (int x = 130; x < 146; ++x) Set(x, y, CellType::kObstacle);
    for (int x = 175; x < 190; ++x) Set(x, y, CellType::kObstacle);
  }

  for (int gate_y : {45, 95, 145, 185}) {
    for (int x = 35; x < 50; ++x) Set(x, gate_y, CellType::kRoad);
    for (int x = 80; x < 96; ++x) Set(x, gate_y + 6, CellType::kRoad);
    for (int x = 130; x < 146; ++x) Set(x, gate_y, CellType::kRoad);
    for (int x = 175; x < 190; ++x) Set(x, gate_y + 4, CellType::kRoad);
  }

  // Exits on borders.
  for (int x : {5, 110, 214}) {
    Set(x, 2, CellType::kDestination);
    Set(x, h_ - 3, CellType::kDestination);
    destinations_.push_back(CellCenter(x, 2));
    destinations_.push_back(CellCenter(x, h_ - 3));
  }
}

void Grid::PaintObstacleRect(int x0, int y0, int x1, int y1, bool obstacle) {
  if (x0 > x1) std::swap(x0, x1);
  if (y0 > y1) std::swap(y0, y1);

  for (int y = y0; y <= y1; ++y) {
    for (int x = x0; x <= x1; ++x) {
      if (!InBounds(x, y)) {
        continue;
      }
      if (obstacle) {
        Set(x, y, CellType::kObstacle);
      } else if (Get(x, y) != CellType::kDestination) {
        Set(x, y, CellType::kRoad);
      }
    }
  }
}

}  // namespace flowforge
