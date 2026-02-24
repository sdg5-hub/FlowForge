#pragma once

#include <SDL.h>

#include "flowforge/render/BitmapFont.h"
#include "flowforge/sim/Simulation.h"
#include "flowforge/util/Types.h"

namespace flowforge {

struct Camera {
  float zoom = 1.0f;
  Vec2 offset{0.0f, 0.0f};
};

class Renderer {
 public:
  Renderer(int width, int height);
  ~Renderer();

  bool Init();
  void Shutdown();

  bool running() const { return running_; }
  void Stop() { running_ = false; }

  Camera& camera() { return camera_; }
  const Camera& camera() const { return camera_; }

  SDL_Window* window() const { return window_; }

  void Begin();
  void Draw(const Simulation& sim, float fps, float speed_mul, bool paused);
  void End();

  Vec2 ScreenToWorld(int sx, int sy, float cell_size) const;

 private:
  int width_;
  int height_;
  bool running_{true};

  SDL_Window* window_{nullptr};
  SDL_Renderer* renderer_{nullptr};
  Camera camera_{};
  BitmapFont font_{};

  SDL_FPoint WorldToScreen(const Vec2& p) const;
  void DrawGrid(const Grid& grid, const MetricsSnapshot& m, const OverlayState& o);
  void DrawLights(const Grid& grid);
  void DrawFlowVectors(const Grid& grid, const Simulation& sim);
  void DrawAgents(const AgentSoA& a, const Simulation& sim);
  void DrawHud(const Simulation& sim, float fps, float speed_mul, bool paused);
};

}  // namespace flowforge
