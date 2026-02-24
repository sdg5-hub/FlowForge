#pragma once

#include <SDL.h>

#include "flowforge/render/Renderer.h"
#include "flowforge/sim/Simulation.h"

namespace flowforge {

struct AppState {
  bool paused = false;
  bool step_once = false;
  float speed_mul = 1.0f;
};

class InputController {
 public:
  void Process(Renderer* renderer, Simulation* sim, AppState* app);

 private:
  bool dragging_ = false;
  int last_x_ = 0;
  int last_y_ = 0;

  bool edit_mode_ = false;
  bool erase_mode_ = false;
  int brush_start_x_ = -1;
  int brush_start_y_ = -1;
};

}  // namespace flowforge
