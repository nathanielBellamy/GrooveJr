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

  bool                             active = false;
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
  Cassette*                        cassette;

  AudioDeck(int deckIndex, AppState* gAppState)
    : deckIndex(deckIndex)
    , gAppState(gAppState)
    , cassette(new Cassette(gAppState))
    {}

  ~AudioDeck() {
    Logging::write(
      Info,
      "AudioDeck::~AudioDeck()",
      "Destroying AudioDeck - deckIndex : " + std::to_string(deckIndex)
    );

    delete cassette;

    Logging::write(
      Info,
      "AudioDeck::~AudioDeck()",
      "Destroyed AudioDeck - deckIndex : " + std::to_string(deckIndex)
    );
  }

  Result setCassetteFromFilePath(const char* filePath) {
    delete cassette;
    cassette = new Cassette(gAppState, filePath);
    frames = cassette->sfInfo.frames;
    inputBuffers[0] = cassette->inputBuffers[0];
    inputBuffers[1] = cassette->inputBuffers[1];
    return OK;
  }
};

} // Audio
} // Gj

#endif //DECKDATA_H
