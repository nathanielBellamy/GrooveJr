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
#include "../enums/PlayState.h"
#include "../db/dto/musicLibrary/DecoratedAudioFile.h"

namespace Gj {
namespace Audio {

constexpr sf_count_t MIN_FADE_IN = 500;
constexpr sf_count_t MIN_FADE_OUT = 500;

struct AudioDeck {
  int                                   deckIndex;
  PlayState                             playState = STOP;
  AppState*                             gAppState;
  mutable sf_count_t                    frameId = 0;
  sf_count_t                            frames = 0; // total # of frames
  sf_count_t                            frameAdvance;
  float                                 gain = 1.0f;
  float*                                inputBuffers[2]{nullptr, nullptr};
  std::optional<Cassette>               cassette;
  std::optional<Db::DecoratedAudioFile> decoratedAudioFile;

  AudioDeck(const int deckIndex, AppState* gAppState)
    : deckIndex(deckIndex)
    , gAppState(gAppState)
    , cassette(std::optional<Cassette>())
    , decoratedAudioFile(std::optional<Db::DecoratedAudioFile>())
    {}

  ~AudioDeck() {
    Logging::write(
      Info,
      "AudioDeck::~AudioDeck()",
      "Destroying AudioDeck - deckIndex : " + std::to_string(deckIndex)
    );

    Logging::write(
      Info,
      "AudioDeck::~AudioDeck()",
      "Destroyed AudioDeck - deckIndex : " + std::to_string(deckIndex)
    );
  }

  Result setCassetteFromDecoratedAudioFile(const Db::DecoratedAudioFile& newDecoratedAudioFile) {
    decoratedAudioFile = std::optional(newDecoratedAudioFile);
    cassette = std::optional(Cassette(gAppState, decoratedAudioFile->audioFile.filePath.c_str()));
    frames = cassette->sfInfo.frames;
    inputBuffers[0] = cassette->inputBuffers[0];
    inputBuffers[1] = cassette->inputBuffers[1];

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

  bool validCassetteLoaded() const {
    if (!cassette)
      return false;

    const bool nonZeroFrames = frames > 0 && frames == cassette->sfInfo.frames;
    // const bool nonBlankCassette = cassette->filePath != "BLANK";
    const bool nonNullInputBuffers =
         inputBuffers[0] != nullptr
      && inputBuffers[1] != nullptr
      && inputBuffers[0] == cassette->inputBuffers[0]
      && inputBuffers[1] == cassette->inputBuffers[1];

    return nonZeroFrames
      // && nonBlankCassette
      && nonNullInputBuffers;
  }

  bool isPlaying() const {
    return playState == PLAY;
  }
};

} // Audio
} // Gj

#endif //DECKDATA_H
