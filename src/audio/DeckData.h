//
// Created by ns on 9/18/25.
//

#ifndef DECKDATA_H
#define DECKDATA_H

#include <sndfile.h>

namespace Gj {
namespace Audio {

struct DeckData {
  sf_count_t                       frameId;
  sf_count_t                       frames { 0 }; // total # of frames
  sf_count_t                       frameAdvance;
  bool                             readComplete;
  float                            volume;
  float                            fadeIn;
  float                            fadeOut;
  float*                           inputBuffers[2]{nullptr, nullptr};
}

} // Audio
} // Gj

#endif //DECKDATA_H
