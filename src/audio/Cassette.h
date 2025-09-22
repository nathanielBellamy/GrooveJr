#ifndef CASSETTE_H
#define CASSETTE_H

#include <array>
#include <vector>

#include <sndfile.hh>

#include "./Constants.h"
#include "./ThreadStatics.h"
#include "../AppState.h"
#include "Logging.h"

namespace Gj {
namespace Audio {

class Cassette
{
  long threadId;
  char const* fileName;
  long initialFrameId;
  AppState* gAppState;
  float* buffer{};
  SF_INFO sfInfo;
  SNDFILE* file{};
  float* inputBuffer{};

  int setupAudioData();

  bool allocateInputBuffers();
  bool populateInputBuffers() const;
  bool setupInputBuffers();

  [[nodiscard]]
  bool deleteBuffers() const;

  public:
    float* inputBuffers[2]{}; // full song audio data

    Cassette(AppState* gAppState);
    ~Cassette();

    void cleanup();
};

} // Audio
} // Gj

#endif