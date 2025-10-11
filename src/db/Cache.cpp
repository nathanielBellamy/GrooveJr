//
// Created by ns on 10/11/25.
//

#include "Cache.h"

namespace Gj {
namespace Db {

Result Cache::loadTracksAll() {
  tracks.clear();
  return dao->trackRepository.loadAll(tracks);
}

} // Db
} // Gj