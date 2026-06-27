//
// Created by ns on 11/14/25.
//

#ifndef GJATOMICSTR_H
#define GJATOMICSTR_H

#include <algorithm>
#include <string>

namespace Gj {

constexpr size_t ATOMIC_STR_SIZE = 512;

struct AtomicStr {
  char value[ATOMIC_STR_SIZE] { ' ' };

  AtomicStr() {}

  AtomicStr(const char* str) {
    assign(str);
  }

  AtomicStr(const std::string& str) {
    assign(str.c_str(), str.size());
  }

  const char* c_str() const {
    return value;
  }

  std::string std_str() const {
    const auto* end = std::find(value, value + ATOMIC_STR_SIZE, '\0');
    return std::string(value, end);
  }

private:
  void assign(const char* str) {
    size_t length = 0;
    while (length < ATOMIC_STR_SIZE - 1 && str[length] != '\0') {
      value[length] = str[length];
      length++;
    }
    value[length] = '\0';
  }

  void assign(const char* str, const size_t length) {
    const auto copyLength = std::min(length, ATOMIC_STR_SIZE - 1);
    for (size_t i = 0; i < copyLength; i++) {
      value[i] = str[i];
    }
    value[copyLength] = '\0';
  }
};


inline std::string operator+(const std::string& lhs, const AtomicStr& rhs) {
  return lhs + rhs.std_str();
}

inline std::string operator+(const AtomicStr& lhs, const std::string& rhs) {
  return lhs.std_str() + rhs;
}

template <class Inspector>
bool inspect(Inspector& f, AtomicStr& x) {
    return f.object(x).fields(
      f.field("value", x.value)
    );
}

}
#endif //ATOMICSTR_H
