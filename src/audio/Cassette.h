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
  char const* filePath;
  long initialFrameId;
  AppState* gAppState;
  float* buffer{};
  SNDFILE* file{};
  float* inputBuffer{};

  int init();

  bool allocateInputBuffers();
  bool populateInputBuffers() const;
  bool setupInputBuffers();

  [[nodiscard]]
  bool deleteBuffers() const;
  Cassette() {};

  public:
    SF_INFO sfInfo;
    float* inputBuffers[2]{}; // full song audio data

    Cassette(AppState* gAppState, const char* filePath);
    ~Cassette();

    void cleanup();

    static Cassette blank() {
      return Cassette();
    }
};

} // Audio
} // Gj

#endif