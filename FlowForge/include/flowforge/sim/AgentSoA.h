#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "flowforge/sim/Config.h"
#include "flowforge/sim/Grid.h"
#include "flowforge/util/Rng.h"
#include "flowforge/util/Types.h"

namespace flowforge {

class AgentSoA {
 public:
  void Resize(std::size_t n);
  void Spawn(const Grid& grid, const SimConfig& cfg, Rng& rng, DemoMode mode);

  std::size_t size() const { return px.size(); }

  std::vector<float> px;
  std::vector<float> py;
  std::vector<float> vx;
  std::vector<float> vy;
  std::vector<float> heading;
  std::vector<float> speed;
  std::vector<std::uint8_t> type;
  std::vector<std::uint16_t> destination;
  std::vector<std::uint8_t> active;
  std::vector<std::uint32_t> id;
};

}  // namespace flowforge
