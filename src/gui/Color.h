//
// Created by ns on 5/16/25.
//

#ifndef GJGUICOLORS_H
#define GJGUICOLORS_H

#include <map>

namespace Gj {
namespace Gui {

enum class GjC {
  DARK_300,
  DARK_200,
  DARK_100,
  LIGHT_300,
  LIGHT_200,
  LIGHT_100
};

class Color {
  public:
    static inline std::map<GjC, std::string> hexs = {
      {GjC::DARK_300, "#2A363B"},
      {GjC::DARK_200, "#E5FCC2"},
      {GjC::DARK_100, "#9DE0AD"},
      {GjC::LIGHT_300, "#45ADA8"},
      {GjC::LIGHT_200, "#547980"},
      {GjC::LIGHT_100, "#594F4F"}
    };

    static std::string toHex(const GjC color) {
      return hexs.find(color)->second;
    };
};

} // Gui
} // Gj

#endif //GJGUICOLORS_H
