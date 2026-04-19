#ifndef CASSETTE_H
#define CASSETTE_H

#include <array>
#include <vector>

#include <sndfile.hh>

#include "../Constants.h"
#include "../../state/Core.h"
#include "../../enums/Result.h"
#include "../Logging.h"

namespace Gj {
namespace Audio {
class Cassette {
  long initialFrameId;
  float* buffer{nullptr};
  SNDFILE* file{};
  float* inputBuffer{nullptr};

  int init();

  bool allocateInputBuffers();

  bool populateInputBuffers() const;

  bool setupInputBuffers();

  [[nodiscard]]
  Result deleteBuffers() const;

public:
  char const* filePath;
  SF_INFO sfInfo;
  float* inputBuffers[2]{nullptr, nullptr}; // full song audio data

  Cassette(const char* filePath);

  Cassette();

  ~Cassette();

  void cleanup();

  static Cassette blank() {
    return Cassette();
  }
};
} // Audio
} // Gj

#endif
