//
// Created by ns on 6/10/25.
//

#ifndef GJAUDIOMATH_H
#define GJAUDIOMATH_H

#include <cmath>

namespace Gj {
namespace Audio {

struct Math {

  static int floatToUInt127(const float val) {
    const auto inter = static_cast<int>(std::floor(val * 100.0f));
    if (inter > 127)
      return 127;
    if (inter < 0)
      return 0;
    return inter;
  };

  static int floatToInt127(const float val) {
    const auto inter = static_cast<int>(std::floor(val * 100.0f));
    if (inter > 127)
      return 127;
    if (inter < -127)
      return -127;
    return inter;
  };

  static float uInt127ToFloat(const int val) {
    if (val < 0)
      return 0.0f;
    if (val > 127)
      return 1.27f;
    return static_cast<float>(val) / 100.0f;
  }

  static float int127ToFloat(const int val) {
    if (val < -127)
      return -1.27f;
    if (val > 127)
      return 1.27f;
    return static_cast<float>(val) / 100.0f;
  }

};

} // Audio
} // Gj

#endif //GJAUDIOMATH_H
