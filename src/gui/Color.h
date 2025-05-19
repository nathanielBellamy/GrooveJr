//
// Created by ns on 5/16/25.
//

#ifndef GJGUICOLORS_H
#define GJGUICOLORS_H

#include <map>

namespace Gj {
namespace Gui {

enum class GjC {
  DARK_500,
  DARK_400,
  DARK_300,
  DARK_200,
  DARK_100,
  LIGHT_400,
  LIGHT_300,
  LIGHT_200,
  LIGHT_100,
  MUTE_BLUE,
  SOLO_YELLOW
};

class Color {
  public:
    static inline std::map<GjC, std::string> hexs = {
      {GjC::DARK_500, "#2D2D2D"},
      {GjC::DARK_400, "#2A363B"},
      {GjC::DARK_300, "#594F4F"},
      {GjC::DARK_200, "#645757"},
      {GjC::DARK_100, "#9DE0AD"},
      {GjC::LIGHT_400, "#00948E"},
      {GjC::LIGHT_300, "#45ADA8"},
      {GjC::LIGHT_200, "#41C2AB"},
      {GjC::LIGHT_100, "#E5EEC1"},
      {GjC::MUTE_BLUE, "#55A0FE"},
      {GjC::SOLO_YELLOW, "#F7DB69"}
    };

    static std::string toHex(const GjC color) {
      return hexs.find(color)->second;
    };
};

} // Gui
} // Gj

#endif //GJGUICOLORS_H
