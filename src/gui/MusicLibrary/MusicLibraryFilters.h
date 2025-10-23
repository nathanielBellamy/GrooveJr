//
// Created by ns on 10/14/25.
//

#ifndef MUSICLIBRARYFILTERS_H
#define MUSICLIBRARYFILTERS_H

#include <map>
#include <string>
#include <vector>

#include "../../enums/Result.h"
#include "../../db/Types.h"

#include "MusicLibraryType.h"

namespace Gj {
namespace Gui {

struct MusicLibraryFilter {
  MusicLibraryType type = NONE;
  std::vector<Db::ID> ids {};
};

struct MusicLibraryFilters {

  std::map<MusicLibraryType, MusicLibraryFilter> filters = {
    {ALBUM, {}},
    {ARTIST, {}},
    {AUDIO_FILE, {}},
    {GENRE, {}},
    {PLAYLIST, {}}
  };

  std::string idSqlArray(const MusicLibraryType type) const {
    const std::vector<Db::ID> ids = filters.at(type).ids;
    std::string res = "(";
    for (size_t i = 0; i < ids.size(); ++i) {
      res += std::to_string(ids[i]);
      if (i != ids.size() - 1)
        res += ", ";
    }
    res += ")";
    return res;
  }

  Result set(const MusicLibraryType type, const uint64_t id) {
    filters.at(type).ids.clear();
    filters.at(type).ids.push_back(id);

    return OK;
  }
};

} // Gui
} // Gj

#endif //FILTERS_H
