//
// Created by ns on 4/19/26.
//

#ifndef GJSTATEAUDIOCORESHADOW_H
#define GJSTATEAUDIOCORESHADOW_H

#include <optional>
#include <sstream>

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
    {0, STOP, 0, std::nullopt}, {1, STOP, 0, std::nullopt}, {2, STOP, 0, std::nullopt}
  };
  DeckIndex deckIndex = 0;
  DeckIndex deckIndexNext = 0;

  std::string toString() const {
    std::ostringstream stream;
    stream << "CoreShadow { decks: [";

    for (int i = 0; i < Gj::Audio::AUDIO_CORE_DECK_COUNT; i++) {
      if (i > 0)
        stream << ", ";

      stream << decks[i].toString();
    }

    stream
        << "], deckIndex: " << deckIndex << std::endl
        << ", deckIndexNext: " << deckIndexNext << std::endl
        << " }";
    return stream.str();
  }
};
} // Audio
} // State
} // Gj

#endif //GJSTATRECORESHADOW_H
