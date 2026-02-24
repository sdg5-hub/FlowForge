#include "flowforge/ui/InputController.h"

namespace flowforge {

void InputController::Process(Renderer* renderer, Simulation* sim, AppState* app) {
  app->step_once = false;

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      renderer->Stop();
      return;
    }

    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          renderer->Stop();
          break;
        case SDLK_SPACE:
          app->paused = !app->paused;
          break;
        case SDLK_n:
          app->step_once = true;
          break;
        case SDLK_EQUALS:
        case SDLK_PLUS:
          app->speed_mul = Clamp(app->speed_mul * 1.25f, 0.25f, 8.0f);
          break;
        case SDLK_MINUS:
          app->speed_mul = Clamp(app->speed_mul * 0.8f, 0.25f, 8.0f);
          break;
        case SDLK_1:
          sim->Reset(DemoMode::kCityRushHour);
          break;
        case SDLK_2:
          sim->Reset(DemoMode::kEvacuation);
          break;
        case SDLK_3:
          sim->Reset(DemoMode::kInteractiveEditing);
          break;
        case SDLK_p:
          sim->SetPathMode(sim->path_mode() == PathMode::kFlowField ? PathMode::kAStarDebug
                                                                     : PathMode::kFlowField);
          break;
        case SDLK_h:
          sim->overlays().heatmap = !sim->overlays().heatmap;
          break;
        case SDLK_f:
          sim->overlays().flow_vectors = !sim->overlays().flow_vectors;
          break;
        case SDLK_c:
          sim->overlays().collision_debug = !sim->overlays().collision_debug;
          break;
        case SDLK_i:
          sim->overlays().agent_ids = !sim->overlays().agent_ids;
          break;
        case SDLK_e:
          edit_mode_ = !edit_mode_;
          break;
        default:
          break;
      }
    }

    if (e.type == SDL_MOUSEWHEEL) {
      renderer->camera().zoom = Clamp(renderer->camera().zoom + 0.08f * static_cast<float>(e.wheel.y),
                                      0.25f, 4.0f);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
      if (edit_mode_) {
        erase_mode_ = false;
        Vec2 w = renderer->ScreenToWorld(e.button.x, e.button.y, sim->cfg().cell_size);
        brush_start_x_ = static_cast<int>(w.x / sim->cfg().cell_size);
        brush_start_y_ = static_cast<int>(w.y / sim->cfg().cell_size);
      } else {
        dragging_ = true;
        last_x_ = e.button.x;
        last_y_ = e.button.y;
      }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT && edit_mode_) {
      erase_mode_ = true;
      Vec2 w = renderer->ScreenToWorld(e.button.x, e.button.y, sim->cfg().cell_size);
      brush_start_x_ = static_cast<int>(w.x / sim->cfg().cell_size);
      brush_start_y_ = static_cast<int>(w.y / sim->cfg().cell_size);
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
      if (e.button.button == SDL_BUTTON_LEFT && dragging_) {
        dragging_ = false;
      }

      if (edit_mode_ && (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT)) {
        Vec2 w = renderer->ScreenToWorld(e.button.x, e.button.y, sim->cfg().cell_size);
        const int ex = static_cast<int>(w.x / sim->cfg().cell_size);
        const int ey = static_cast<int>(w.y / sim->cfg().cell_size);
        sim->SetObstacleBrush(brush_start_x_, brush_start_y_, ex, ey, !erase_mode_);
      }
    }

    if (e.type == SDL_MOUSEMOTION && dragging_) {
      const int dx = e.motion.x - last_x_;
      const int dy = e.motion.y - last_y_;
      renderer->camera().offset.x += static_cast<float>(dx) / renderer->camera().zoom;
      renderer->camera().offset.y += static_cast<float>(dy) / renderer->camera().zoom;
      last_x_ = e.motion.x;
      last_y_ = e.motion.y;
    }
  }

  const Uint8* keys = SDL_GetKeyboardState(nullptr);
  const float pan = 10.0f / renderer->camera().zoom;
  if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) renderer->camera().offset.y += pan;
  if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) renderer->camera().offset.y -= pan;
  if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) renderer->camera().offset.x += pan;
  if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) renderer->camera().offset.x -= pan;
}

}  // namespace flowforge
