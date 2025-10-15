//
// Created by ns on 10/14/25.
//

#ifndef MUSICLIBRARYFILTERS_H
#define MUSICLIBRARYFILTERS_H

#include <vector>

namespace Gj {
namespace Gui {

enum FilterBy {
  ALBUM,
  ARTIST,
  GENRE,
  PLAYLIST,
  TRACK
};

struct MusicLibraryFilters {
  FilterBy filterBy;
  std::vector<uint64_t> ids {};

  std::string idSqlArray() const {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < ids.size(); ++i) {
      oss << ids[i];
      if (i != ids.size() - 1) {
        oss << ", ";
      }
    }
    oss << ")";
    return oss.str();
  }
};

} // Gui
} // Gj

#endif //FILTERS_H
