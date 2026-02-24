#pragma once

#include <SDL.h>
#include <string>

namespace flowforge {

class BitmapFont {
 public:
  void DrawText(SDL_Renderer* r, int x, int y, const std::string& text, SDL_Color c, int scale = 2) const;
};

}  // namespace flowforge
