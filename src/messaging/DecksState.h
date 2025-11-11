//
// Created by ns on 11/10/25.
//

#ifndef GJDECKSSTATE_H
#define GJDECKSSTATE_H

#include "../audio/Constants.h"
#include "../db/Types.h"

namespace Gj {

struct DecksState {
  Db::ID audioFileIds[Audio::AUDIO_CORE_DECK_COUNT];
  int currentDeckIdx;
};

template <class Inspector>
bool inspect(Inspector& f, DecksState& x) {
    return f.object(x).fields(
      f.field("audioFileIds", x.audioFileIds),
      f.field("currentDeckIdx", x.currentDeckIdx)
    );
}

} // Gj

#endif //DECKSSTATE_H
