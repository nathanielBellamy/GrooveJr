//
// Created by ns on 10/19/25.
//

#ifndef QUEUE_H
#define QUEUE_H

#include "../../../types/Types.h"

namespace Gj {
namespace Db {

struct Queue {
  ID id;
  ID audioFileId;
  TrackNumber trackNumber;

  Queue(ID id, ID audioFileId, TrackNumber trackNumber)
    : id(id)
    , audioFileId(audioFileId)
    , trackNumber(trackNumber)
    {}

  Queue(ID audioFileId, TrackNumber trackNumber)
    : id(0)
    , audioFileId(audioFileId)
    , trackNumber(trackNumber)
    {}
};

} // Db
} // Gj

#endif //QUEUE_H
