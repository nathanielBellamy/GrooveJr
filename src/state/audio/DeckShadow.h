//
// Created by ns on 4/19/26.
//

#ifndef GJSTATEAUDIODECKSHADOW_H
#define GJSTATEAUDIODECKSHADOW_H

#include <optional>
#include <sndfile.h>

#include "../../enums/PlayState.h"
#include "../../db/dto/musicLibrary/DecoratedAudioFile.h"
#include "../../types/Types.h"

namespace Gj {
namespace State {
namespace Audio {
struct DeckShadow {
  DeckIndex deckIndex = 0;
  PlayState playState = STOP;
  std::optional<Db::DecoratedAudioFile> decoratedAudioFile;
};
} // Audio
} // State
} // Gj


#endif //DECKSHADOW_H
