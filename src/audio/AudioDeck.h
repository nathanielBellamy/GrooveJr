//
// Created by ns on 9/18/25.
//

#ifndef DECKDATA_H
#define DECKDATA_H

#include <optional>

#include <sndfile.h>

#include "Cassette.h"

#include "../AppState.h"
#include "../Logging.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {

  struct AudioDeck {
  int                              deckIndex;
  AppState*                        gAppState;
  sf_count_t                       frameId;
  sf_count_t                       frames { 0 }; // total # of frames
  sf_count_t                       frameAdvance;
  bool                             readComplete;
  float                            volume;
  float                            fadeIn;
  float                            fadeOut;
  float*                           inputBuffers[2]{nullptr, nullptr};
  Cassette                         cassette;

  AudioDeck(int deckIndex, AppState* gAppState)
    : deckIndex(deckIndex)
    , gAppState(gAppState)
    , cassette(Cassette::blank())
    {}

  Result setCassetteFromFilePath(const char* filePath) {
    cassette = Cassette(gAppState, filePath);
    return OK;
  }
};

} // Audio
} // Gj

#endif //DECKDATA_H
