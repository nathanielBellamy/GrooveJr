//
// Created by ns on 10/14/25.
//

#ifndef MUSICLIBRARYFILTERS_H
#define MUSICLIBRARYFILTERS_H

#include <vector>
#include <string>

#include "../../enums/Result.h"

#include "MusicLibraryType.h"

namespace Gj {
namespace Gui {

struct MusicLibraryFilters {
  MusicLibraryType type = NONE;
  std::vector<uint64_t> ids {};

  std::string idSqlArray() const {
    std::string res = "(";
    for (size_t i = 0; i < ids.size(); ++i) {
      res += std::to_string(ids[i]);
      if (i != ids.size() - 1)
        res += ", ";
    }
    res += ")";
    return res;
  }

  Result set(MusicLibraryType newFilterBy, uint64_t id) {
    type = newFilterBy;
    ids.clear();
    ids.push_back(id);

    return OK;
  }
};

} // Gui
} // Gj

#endif //FILTERS_H
