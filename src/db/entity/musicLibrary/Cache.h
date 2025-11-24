//
// Created by ns on 10/29/25.
//

#ifndef GJDBCACHE_H
#define GJDBCACHE_H

#include "../../../types/Types.h"

namespace Gj {
namespace Db {

struct Cache {
  ID id;
  ID audioFileId;

  Cache(const ID id, const ID audioFileId)
    : id(id)
    , audioFileId(audioFileId)
    {}

  Cache(const ID audioFileId)
    : id(0)
    , audioFileId(audioFileId)
    {}
};

} // Db
} // Gj

#endif //GJDBCACHE_H
