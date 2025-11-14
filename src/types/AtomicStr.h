//
// Created by ns on 11/14/25.
//

#ifndef GJATOMICSTR_H
#define GJATOMICSTR_H

#include <string>

namespace Gj {

constexpr size_t ATOMIC_STR_SIZE = 512;

struct AtomicStr {
  char value[ATOMIC_STR_SIZE];

  AtomicStr(const char* str) {
    for (int i = 0; str[i] != '\0' && i < ATOMIC_STR_SIZE; i++) {
      value[i] = str[i];
    }
  }

  AtomicStr(const std::string& str) {
    for (int i = 0; i < str.size() && i < ATOMIC_STR_SIZE; i++) {
      value[i] = str[i];
    }
  }

  const char* c_str() const {
    return value;
  }

  std::string std_str() const {
    return std::string(value);
  }
};

}

#endif //ATOMICSTR_H
