#include "flowforge/render/Renderer.h"

#include <algorithm>

namespace flowforge {

Renderer::Renderer(int width, int height) : width_(width), height_(height) {}
Renderer::~Renderer() { Shutdown(); }

bool Renderer::Init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return false;

  window_ = SDL_CreateWindow("FlowForge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_,
                             height_, SDL_WINDOW_SHOWN);
  if (!window_) return false;

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer_) return false;

  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
  return true;
}

void Renderer::Shutdown() {
  if (renderer_) SDL_DestroyRenderer(renderer_);
  renderer_ = nullptr;
  if (window_) SDL_DestroyWindow(window_);
  window_ = nullptr;
  SDL_Quit();
}

void Renderer::Begin() {
  SDL_SetRenderDrawColor(renderer_, 10, 14, 18, 255);
  SDL_RenderClear(renderer_);
}

void Renderer::Draw(const Simulation& sim, float fps, float speed_mul, bool paused) {
  DrawGrid(sim.grid(), sim.metrics(), sim.overlays());
  DrawLights(sim.grid());
  if (sim.overlays().flow_vectors) {
    DrawFlowVectors(sim.grid(), sim);
  }
  DrawAgents(sim.agents(), sim);
  DrawHud(sim, fps, speed_mul, paused);
}

void Renderer::End() { SDL_RenderPresent(renderer_); }

SDL_FPoint Renderer::WorldToScreen(const Vec2& p) const {
  return {(p.x + camera_.offset.x) * camera_.zoom, (p.y + camera_.offset.y) * camera_.zoom};
}

Vec2 Renderer::ScreenToWorld(int sx, int sy, float) const {
  return {static_cast<float>(sx) / camera_.zoom - camera_.offset.x,
          static_cast<float>(sy) / camera_.zoom - camera_.offset.y};
}

void Renderer::DrawGrid(const Grid& grid, const MetricsSnapshot& m, const OverlayState& o) {
  const float cs = grid.cell_size();
  for (int y = 0; y < grid.h(); ++y) {
    for (int x = 0; x < grid.w(); ++x) {
      const CellType c = grid.Get(x, y);
      SDL_FPoint p = WorldToScreen({x * cs, y * cs});
      SDL_FRect r{p.x, p.y, cs * camera_.zoom + 0.5f, cs * camera_.zoom + 0.5f};

      Uint8 rr = 28, gg = 34, bb = 40;
      if (c == CellType::kRoad) {
        rr = 58;
        gg = 84;
        bb = 104;
      } else if (c == CellType::kIntersection) {
        rr = 74;
        gg = 104;
        bb = 128;
      } else if (c == CellType::kDestination) {
        rr = 232;
        gg = 184;
        bb = 56;
      }

      if (o.heatmap) {
        const float d = m.density[static_cast<std::size_t>(grid.Index(x, y))];
        const float t = Clamp(d / 8.0f, 0.0f, 1.0f);
        rr = static_cast<Uint8>(rr + 170 * t);
        gg = static_cast<Uint8>(gg * (1.0f - 0.5f * t));
        bb = static_cast<Uint8>(bb * (1.0f - 0.8f * t));
      }

      SDL_SetRenderDrawColor(renderer_, rr, gg, bb, 255);
      SDL_RenderFillRectF(renderer_, &r);
    }
  }
}

void Renderer::DrawLights(const Grid& grid) {
  for (const auto& l : grid.lights()) {
    const SDL_FPoint p = WorldToScreen(grid.CellCenter(l.x, l.y));
    const float s = 3.5f * camera_.zoom;

    SDL_SetRenderDrawColor(renderer_, l.VerticalGreen() ? 60 : 230, l.VerticalGreen() ? 230 : 60,
                           60, 220);
    SDL_FRect r{p.x - s, p.y - s, s * 2.0f, s * 2.0f};
    SDL_RenderFillRectF(renderer_, &r);
  }
}

void Renderer::DrawFlowVectors(const Grid& grid, const Simulation& sim) {
  const AgentSoA& agents = sim.agents();
  SDL_SetRenderDrawColor(renderer_, 140, 200, 255, 130);

  for (std::size_t i = 0; i < agents.size(); i += 48) {
    if (!agents.active[i]) continue;
    const Vec2 p{agents.px[i], agents.py[i]};
    const Vec2 v = {agents.vx[i], agents.vy[i]};
    const Vec2 d = v.LengthSq() > 0.01f ? v.Normalized() : Vec2{0.0f, 0.0f};
    const SDL_FPoint a = WorldToScreen(p);
    const SDL_FPoint b = WorldToScreen(p + d * (grid.cell_size() * 1.2f));
    SDL_RenderDrawLineF(renderer_, a.x, a.y, b.x, b.y);
  }
}

void Renderer::DrawAgents(const AgentSoA& a, const Simulation& sim) {
  (void)sim;
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (!a.active[i]) continue;
    const AgentType t = static_cast<AgentType>(a.type[i]);
    const float rad = (t == AgentType::kVehicle ? 2.8f : 1.9f) * camera_.zoom;
    const SDL_FPoint p = WorldToScreen({a.px[i], a.py[i]});

    if (t == AgentType::kVehicle) {
      SDL_SetRenderDrawColor(renderer_, 255, 140, 80, 215);
      SDL_FRect box{p.x - rad * 1.2f, p.y - rad * 0.8f, rad * 2.4f, rad * 1.6f};
      SDL_RenderFillRectF(renderer_, &box);
    } else {
      SDL_SetRenderDrawColor(renderer_, 120, 240, 170, 200);
      for (int dy = -2; dy <= 2; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {
          const float fx = dx * rad * 0.4f;
          const float fy = dy * rad * 0.4f;
          if (fx * fx + fy * fy <= rad * rad) {
            SDL_RenderDrawPointF(renderer_, p.x + fx, p.y + fy);
          }
        }
      }
    }

    if (sim.overlays().collision_debug) {
      SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 40);
      SDL_FRect rr{p.x - 6.0f * camera_.zoom, p.y - 6.0f * camera_.zoom, 12.0f * camera_.zoom,
                   12.0f * camera_.zoom};
      SDL_RenderDrawRectF(renderer_, &rr);
    }

    if (sim.overlays().agent_ids && i < 200) {
      font_.DrawText(renderer_, static_cast<int>(p.x + 2), static_cast<int>(p.y - 5),
                     std::to_string(a.id[i]), {255, 255, 255, 180}, 1);
    }
  }
}

void Renderer::DrawHud(const Simulation& sim, float fps, float speed_mul, bool paused) {
  const SDL_FRect bg{12.0f, 12.0f, 820.0f, 76.0f};
  SDL_SetRenderDrawColor(renderer_, 8, 12, 16, 188);
  SDL_RenderFillRectF(renderer_, &bg);

  (void)speed_mul;
  (void)paused;
  const auto& m = sim.metrics();
  std::string line = "FPS " + std::to_string(static_cast<int>(fps)) + " | AG " +
                     std::to_string(m.active_agents) + " | SPD " + std::to_string(m.avg_speed).substr(0, 4) +
                     " | CON " + std::to_string(static_cast<int>(m.congestion * 100.0f)) + "%";
  if (m.evac_time > 0.0f) {
    line += " | EVAC " + std::to_string(m.evac_time).substr(0, 4) + "s";
  }
  font_.DrawText(renderer_, 22, 24, line, {235, 240, 245, 255}, 2);

  std::string mode = "MODE ";
  if (sim.demo_mode() == DemoMode::kCityRushHour) mode += "CITY";
  if (sim.demo_mode() == DemoMode::kEvacuation) mode += "EVAC";
  if (sim.demo_mode() == DemoMode::kInteractiveEditing) mode += "EDIT";
  mode += " | PATH ";
  mode += (sim.path_mode() == PathMode::kFlowField ? "FLOW" : "ASTAR");
  font_.DrawText(renderer_, 22, 50, mode, {180, 210, 250, 255}, 2);

  SDL_SetWindowTitle(window_,
                     ("FlowForge | " + line + " | Frame " +
                      std::to_string(static_cast<double>(sim.frame_ns()) / 1000000.0).substr(0, 5) +
                      "ms | NQ " +
                      std::to_string(static_cast<double>(sim.neighbor_query_ns()) / 1000000.0).substr(0, 5) +
                      "ms")
                         .c_str());

}

}  // namespace flowforge
