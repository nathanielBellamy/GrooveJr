//
// Created by ns on 5/26/25.
//

#ifndef EFFECT_H
#define EFFECT_H

#include <string>

#include <sqlite3.h>

namespace Gj {
namespace Db {

struct Effect {

  int id;
  std::string filePath;
  std::string name;
  int state; // TODO
  int channelIndex;
  int effectIndex;

  Effect(int id, std::string filePath, std::string name, int state, int channelIndex, int effectIndex);
  static Effect deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //EFFECT_H
