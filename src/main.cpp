#include <SDL.h>

#include <cmath>

#include "flowforge/render/Renderer.h"
#include "flowforge/sim/Simulation.h"
#include "flowforge/ui/InputController.h"

int main() {
  flowforge::SimConfig cfg;
  cfg.agent_count = 10000;

  flowforge::Simulation sim(cfg);
  flowforge::Renderer renderer(1520, 920);
  flowforge::InputController input;
  flowforge::AppState app;

  if (!renderer.Init()) {
    return 1;
  }

  Uint64 prev = SDL_GetPerformanceCounter();
  const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
  double accumulator = 0.0;

  while (renderer.running()) {
    input.Process(&renderer, &sim, &app);

    const Uint64 now = SDL_GetPerformanceCounter();
    const double dt = static_cast<double>(now - prev) / freq;
    prev = now;
    accumulator += dt * app.speed_mul;

    if (app.step_once) {
      sim.Step();
    } else if (!app.paused) {
      while (accumulator >= cfg.fixed_dt) {
        sim.Step();
        accumulator -= cfg.fixed_dt;
      }
    }

    const float fps = dt > 0.0 ? static_cast<float>(1.0 / dt) : 0.0f;
    renderer.Begin();
    renderer.Draw(sim, fps, app.speed_mul, app.paused);
    renderer.End();
  }

  renderer.Shutdown();
  return 0;
}
