#include "flowforge/render/BitmapFont.h"

#include <unordered_map>

namespace flowforge {

void BitmapFont::DrawText(SDL_Renderer* r, int x, int y, const std::string& text, SDL_Color c,
                          int scale) const {
  static const std::unordered_map<char, std::string> kGlyph = {
      {'0', "111101101101111"}, {'1', "010110010010111"}, {'2', "111001111100111"},
      {'3', "111001111001111"}, {'4', "101101111001001"}, {'5', "111100111001111"},
      {'6', "111100111101111"}, {'7', "111001001001001"}, {'8', "111101111101111"},
      {'9', "111101111001111"}, {'A', "111101111101101"}, {'B', "110101110101110"},
      {'C', "111100100100111"}, {'D', "110101101101110"}, {'E', "111100110100111"},
      {'F', "111100110100100"}, {'G', "111100101101111"}, {'H', "101101111101101"},
      {'I', "111010010010111"}, {'L', "100100100100111"}, {'N', "101111111111101"},
      {'O', "111101101101111"}, {'P', "111101111100100"}, {'R', "110101110101101"},
      {'S', "111100111001111"}, {'T', "111010010010010"}, {'U', "101101101101111"},
      {'V', "101101101101010"}, {'X', "101101010101101"}, {'Y', "101101010010010"},
      {'Z', "111001010100111"}, {' ', "000000000000000"}, {':', "000010000010000"},
      {'.', "000000000000010"}, {'|', "010010010010010"}, {'%', "101001010100101"},
      {'/', "001001010100100"}, {'-', "000000111000000"}, {'+', "000010111010000"}};

  SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
  int cursor = x;

  for (char ch : text) {
    char up = (ch >= 'a' && ch <= 'z') ? static_cast<char>(ch - 32) : ch;
    const auto it = kGlyph.find(up);
    const std::string bits = (it != kGlyph.end()) ? it->second : kGlyph.at(' ');

    for (int row = 0; row < 5; ++row) {
      for (int col = 0; col < 3; ++col) {
        if (bits[static_cast<std::size_t>(row * 3 + col)] == '1') {
          SDL_Rect px{cursor + col * scale, y + row * scale, scale, scale};
          SDL_RenderFillRect(r, &px);
        }
      }
    }
    cursor += 4 * scale;
  }
}

}  // namespace flowforge
