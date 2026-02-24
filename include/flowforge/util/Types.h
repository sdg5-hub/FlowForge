#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace flowforge {

struct Vec2 {
  float x{0.0f};
  float y{0.0f};

  Vec2() = default;
  Vec2(float px, float py) : x(px), y(py) {}

  Vec2 operator+(const Vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
  Vec2 operator-(const Vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
  Vec2 operator*(float s) const { return {x * s, y * s}; }
  Vec2 operator/(float s) const { return {x / s, y / s}; }

  Vec2& operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  float LengthSq() const { return x * x + y * y; }
  float Length() const { return std::sqrt(LengthSq()); }

  Vec2 Normalized() const {
    const float len = Length();
    if (len < 1e-6f) {
      return {0.0f, 0.0f};
    }
    return {x / len, y / len};
  }
};

inline float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
inline float Clamp(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }

enum class AgentType : std::uint8_t { kPedestrian = 0, kVehicle = 1 };
enum class PathMode : std::uint8_t { kFlowField = 0, kAStarDebug = 1 };
enum class DemoMode : std::uint8_t { kCityRushHour = 0, kEvacuation = 1, kInteractiveEditing = 2 };

enum class CellType : std::uint8_t {
  kRoad = 0,
  kObstacle = 1,
  kIntersection = 2,
  kDestination = 3,
};

}  // namespace flowforge
