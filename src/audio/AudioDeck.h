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

constexpr sf_count_t MIN_FADE_IN = 500;
constexpr sf_count_t MIN_FADE_OUT = 500;

struct AudioDeck {

  bool                             active = false;
  int                              deckIndex;
  AppState*                        gAppState;
  mutable sf_count_t               frameId = 0;
  sf_count_t                       frames = 0; // total # of frames
  sf_count_t                       frameAdvance;
  float                            volume;
  float                            fadeIn = 0.0f;
  float                            fadeOut = 1.0f;
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

    if (cassette == nullptr) {
      std::cout << "OH NO - NULL CASSETTE" << std::endl;
    }

    Logging::write(
      Info,
      "Audio::AudioDeck::setCassetteFromFilePath",
      "Added Cassette to deck : " + std::to_string(deckIndex) + ". Identified deck and cassette inputBuffers."
    );
    return OK;
  }

  bool isCrossfadeStart() const {
      return frameId < gAppState->getCrossfade();
  }

  bool isCrossfadeEnd() const {
    return frameId > frames - gAppState->getCrossfade();
  }

  bool isFadeIn() const {
    return frameId < std::max(gAppState->getCrossfade(), MIN_FADE_IN);
  }

  bool isFadeOut() const {
    return frameId > std::min(frames - gAppState->getCrossfade(), frames - MIN_FADE_OUT);
  }
};

} // Audio
} // Gj

#endif //DECKDATA_H
