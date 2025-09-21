#include "./Cassette.h"

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

Cassette::Cassette(actor_system& actorSystem, AppState* gAppState, Mixer* mixer)
  : actorSystem(actorSystem)
  , threadId(ThreadStatics::incrThreadId())
  , fileName(ThreadStatics::getFilePath())
  , gAppState(gAppState)
  , mixer(mixer)
  , sfInfo()
  {

  if (int audioDataRes = setupAudioData(); audioDataRes > 0) {
    Logging::write(
      Error,
      "Audio::Cassette::Cassette",
      "Unable to setup AudioData - status: " + std::to_string(audioDataRes)
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - AudioData status: " + std::to_string(audioDataRes)
    );
  }

  Logging::write(
    Info,
    "Audio::Cassette::Cassette",
    "Cassette initialized"
  );
}

Cassette::~Cassette() {
  Logging::write(
    Info,
    "Audio::Cassette::~Cassette",
    "Destroying Cassette"
  );

  cleanup();

  Logging::write(
    Info,
    "Audio::Cassette::~Cassette",
    "Destroyed Cassette"
  );
}

void Cassette::cleanup() {
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Freeing resources for file " + std::string(fileName)
  );

  if (!deleteBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::cleanup",
      "Unable to free buffers"
    );
  }
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Deleted buffers"
  );

  sf_close(file);
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Done freeing resources for file" + std::string(fileName)
  );
};

int Cassette::setupAudioData() {
  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Setting up Cassette"
  );

  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfInfo.format = 0;
  file = sf_open(fileName, SFM_READ, &sfInfo);

  if (file == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to open input file : sf : " + std::string(sf_strerror(nullptr))
    );
    return 1;
  };

  // update plugin effects with info about audio to be processed
  if (mixer->setSampleRate(sfInfo.samplerate) != OK) {
    Logging::write(
      Warning,
      "Audio::Cassette::setupAudioData",
      "Unable to set sample rate: " + std::to_string(sfInfo.samplerate)
    );
  }

  // Allocate memory for data
  buffer = new float[sfInfo.frames * sfInfo.channels];
  if (!buffer) {
      Logging::write(
        Error,
        "Audio::Cassette::setupAudioData",
        "Cannot allocate memory for raw audio buffer"
      );
    return 2;
  }

  // Read the audio data into buffer
  if (const auto readCount = sf_read_float(file, buffer, sfInfo.frames * sfInfo.channels); readCount != sfInfo.frames * sfInfo.channels) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to read file: " + std::string(fileName)
    );
    return 3;
  }

  if (!setupInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to setup inputBuffers"
    );
    return 4;
  };

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Successfully setup AudioData."
  );

  return 0;
}

bool Cassette::allocateInputBuffers() {
  // left and right channels held in contiguous memory
  inputBuffer = new float[2 * sfInfo.frames];
  inputBuffers[0] = inputBuffer;
  inputBuffers[1] = inputBuffer + sfInfo.frames;

  if (inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::allocateInputBuffers",
      "Unable to allocate memory for individual Cassette.inputBuffers"
    );
    return false;
  }

  return true;
}

bool Cassette::populateInputBuffers() const {
  if (buffer == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::populateInputBuffers",
      "Unable to populate input buffers - raw audio buffer is null"
    );
    return false;
  }

  if (inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::populateInputBuffers",
      "Unable to populate input buffers - buffers are null"
    );
    return false;
  }

  // de-interlace audio into shared input buffers
  for (int i = 0; i < sfInfo.frames; i++) {
    inputBuffers[0][i] = buffer[2 * i];
    inputBuffers[1][i] = buffer[2 * i + 1];
  }

  return true;
}

bool Cassette::setupInputBuffers() {
  if (!allocateInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupInputBuffers",
      "Unable to allocate input buffers."
    );
  }

  if (!populateInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupInputBuffers",
      "Unable to populate input buffers."
    );
    return false;
  };

  return true;
}

bool Cassette::deleteBuffers() const {
  Logging::write(
    Info,
    "Audio::Cassette::deleteBuffers",
    "Deleting Cassette buffers."
  );

  try {
    delete[] buffer;
    delete[] inputBuffer;
  } catch (...) {
    Logging::write(
        Error,
        "Audio::Cassette::freeBuffers",
        "Unable to delete Cassette buffers"
    );
    return false;
  }

  Logging::write(
    Info,
    "Audio::Cassette::freeBuffers",
    "Done delete Cassette buffers."
  );

  return true;
}

} // Audio
} // Gj
