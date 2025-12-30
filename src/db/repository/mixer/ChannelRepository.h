//
// Created by ns on 6/7/25.
//

#ifndef CHANNELREPOSITORY_H
#define CHANNELREPOSITORY_H

#include <sqlite3.h>

#include "../../../state/AppState.h"
#include "../../Logging.h"
#include "../../entity/mixer/ChannelEntity.h"

namespace Gj {
namespace Db {
class ChannelRepository {
  sqlite3** db;
  AppState* gAppState;

public:
  explicit ChannelRepository(sqlite3** db, AppState* gAppState);

  int save(const ChannelEntity& channelEntity) const;
};
} // Gj
} // Db


#endif //CHANNELREPOSITORY_H
