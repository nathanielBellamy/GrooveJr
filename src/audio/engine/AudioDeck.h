//
// Created by ns on 9/18/25.
//

#ifndef AUDIODECK_H
#define AUDIODECK_H

#include <optional>

#include <sndfile.h>

#include "Cassette.h"

#include "../../state/Core.h"
#include "../Logging.h"
#include "../../enums/Result.h"
#include "../../enums/PlayState.h"
#include "../../db/dto/musicLibrary/DecoratedAudioFile.h"
#include "../../types/Types.h"

namespace Gj {
namespace Audio {
constexpr sf_count_t MIN_FADE_IN = 500;
constexpr sf_count_t MIN_FADE_OUT = 500;

struct AudioDeck {
  DeckIndex deckIndex;
  PlayState playState = STOP;
  State::Core* stateCore;
  mutable sf_count_t frameId = 0;
  sf_count_t frames = 0; // total # of frames
  sf_count_t frameAdvance;
  float gain = 1.0f;
  float* inputBuffers[2]{nullptr, nullptr};
  Cassette* cassette;
  std::optional<Db::DecoratedAudioFile> decoratedAudioFile;

  AudioDeck(const DeckIndex deckIndex, State::Core* stateCore)
  : deckIndex(deckIndex)
    , stateCore(stateCore)
    , cassette(new Cassette(stateCore))
    , decoratedAudioFile(std::optional<Db::DecoratedAudioFile>()) {
  }

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
    delete cassette;
    cassette = new Cassette(stateCore, decoratedAudioFile->audioFile.filePath.c_str());
    frames = cassette->sfInfo.frames;
    inputBuffers[0] = cassette->inputBuffers[0];
    inputBuffers[1] = cassette->inputBuffers[1];

    Logging::write(
      Info,
      "Audio::AudioDeck::setCassetteFromDecoratedAudioFile",
      "Added Cassette to deck : " + std::to_string(deckIndex) + ". Identified deck and cassette inputBuffers."
    );
    return OK;
  }

  bool isCrossfadeStart() const {
    return frameId < stateCore->getCrossfade();
  }

  bool isCrossfadeEnd() const {
    return frameId > frames - stateCore->getCrossfade();
  }

  bool isFadeIn() const {
    return frameId < std::max(stateCore->getCrossfade(), MIN_FADE_IN);
  }

  bool isFadeOut() const {
    return frameId > std::min(frames - stateCore->getCrossfade(), frames - MIN_FADE_OUT);
  }

  bool hasValidCassetteLoaded() const {
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
    return hasValidCassetteLoaded() && playState == PLAY;
  }
};
} // Audio
} // Gj

#endif //AUDIODECK_H
