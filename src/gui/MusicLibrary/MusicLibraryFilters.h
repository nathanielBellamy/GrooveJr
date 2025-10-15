//
// Created by ns on 10/14/25.
//

#ifndef MUSICLIBRARYFILTERS_H
#define MUSICLIBRARYFILTERS_H

#include <vector>
#include <string>

#include "../../enums/Result.h"

namespace Gj {
namespace Gui {

enum FilterBy {
  ALBUM,
  ARTIST,
  GENRE,
  NONE,
  PLAYLIST,
  TRACK
};

struct MusicLibraryFilters {
  FilterBy filterBy = NONE;
  std::vector<uint64_t> ids {};

  std::string idSqlArray() const {
    std::string oss = "(";
    for (size_t i = 0; i < ids.size(); ++i) {
      oss += std::to_string(ids[i]);
      if (i != ids.size() - 1)
        oss += ", ";
    }
    oss += ")";
    return oss;
  }

  Result set(FilterBy newFilterBy, uint64_t id) {
    filterBy = newFilterBy;
    ids.clear();
    ids.push_back(id);

    return OK;
  }
};

} // Gui
} // Gj

#endif //FILTERS_H
