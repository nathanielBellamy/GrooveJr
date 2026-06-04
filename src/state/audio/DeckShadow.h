//
// Created by ns on 4/19/26.
//

#ifndef GJSTATEAUDIODECKSHADOW_H
#define GJSTATEAUDIODECKSHADOW_H

#include <optional>
#include <sstream>
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
  FrameID frameId = 0;
  std::optional<Db::DecoratedAudioFile> decoratedAudioFile;

  std::string toString() const {
    const auto playStateToString = [](const PlayState value) {
      switch (value) {
        case STOP:
          return std::string("STOP");
        case PLAY:
          return std::string("PLAY");
        case PAUSE:
          return std::string("PAUSE");
        case RW:
          return std::string("RW");
        case FF:
          return std::string("FF");
      }
      return std::string("UNKNOWN");
    };

    std::ostringstream stream;
    stream
        << "DeckShadow { "
        << "deckIndex: " << deckIndex << std::endl
        << ", playState: " << playStateToString(playState) << std::endl
        << ", frameId: " << frameId << std::endl
        << ", decoratedAudioFile: "
        << (decoratedAudioFile.has_value() ? decoratedAudioFile->toString() : "nullopt") << std::endl
        << " }";
    return stream.str();
  }
};
} // Audio
} // State
} // Gj


#endif //DECKSHADOW_H
