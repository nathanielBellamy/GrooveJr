//
// Created by ns on 4/19/26.
//

#ifndef GJSTATEAUDIOCORESHADOW_H
#define GJSTATEAUDIOCORESHADOW_H

#include <optional>

#include <sndfile.h>

#include "../../audio/Constants.h"
#include "../../enums/PlayState.h"
#include "../../types/Types.h"
#include "DeckShadow.h"

namespace Gj {
namespace State {
namespace Audio {
struct CoreShadow {
  DeckShadow decks[Gj::Audio::AUDIO_CORE_DECK_COUNT]{
    {0, STOP, std::nullopt}, {1, STOP, std::nullopt}, {2, STOP, std::nullopt}
  };
  DeckIndex deckIndex = 0;
  DeckIndex deckIndexNext = 0;
};
} // Audio
} // State
} // Gj

#endif //GJSTATRECORESHADOW_H
