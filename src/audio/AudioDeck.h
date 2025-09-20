//
// Created by ns on 9/18/25.
//

#ifndef DECKDATA_H
#define DECKDATA_H

#include <sndfile.h>

#include "../Logging.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {
  // forward decl
  class Cassette;

  struct AudioDeck {
  int                              deckIndex;
  sf_count_t                       frameId;
  sf_count_t                       frames { 0 }; // total # of frames
  sf_count_t                       frameAdvance;
  bool                             readComplete;
  float                            volume;
  float                            fadeIn;
  float                            fadeOut;
  float*                           inputBuffers[2]{nullptr, nullptr};
  Cassette*                        cassette;

  AudioDeck(int deckIndex)
    : deckIndex(deckIndex)
    {}

  Result setCassette(Cassette* newCassette) {
    cassette = newCassette;
    if (cassette == nullptr)
      return ERROR;

    return OK;
  }
};

} // Audio
} // Gj

#endif //DECKDATA_H
