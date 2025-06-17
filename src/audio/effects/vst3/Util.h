//
// Created by ns on 6/16/25.
//

#ifndef GJAUDIOVST3UTIL_H
#define GJAUDIOVST3UTIL_H

#include "public.sdk/source/vst/utility/memoryibstream.h"

#include "../../../enums/Result.h"
#include "../../Logging.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

using namespace Steinberg;

struct Util {

  static Result getStreamSize(IBStream* stream, int64* size) {
    int64 start = 0;
    if (const auto seekRes = stream->seek(0, IBStream::kIBSeekSet, &start); seekRes == kResultOk) {
      Logging::write(
        Info,
        "Audio::Mixer::getStreamSize",
        "Found start of stream at: " + std::to_string(start)
      );
    } else {
      Logging::write(
        Error,
        "Audio::Mixer::getStreamSize",
        "Could not seek to beginning of stream. Code: " + std::to_string(seekRes)
      );

      *size = 0;
      return ERROR;
    }

    Steinberg::int64 end = 0;
    if (const auto seekRes = stream->seek(-1, IBStream::kIBSeekEnd, &end); seekRes == kResultOk) {
      Logging::write(
        Info,
        "Audio::Mixer::getStreamSize",
        "Found end of stream at: " + std::to_string(end)
      );
    } else {
      Logging::write(
        Error,
        "Audio::Mixer::getStreamSize",
        "Could not seek to end of stream. Code: " + std::to_string(seekRes)
      );
      *size = 0;
      return ERROR;
    }

    // Restore the original position
    stream->seek(start, IBStream::kIBSeekSet, nullptr);

    *size = end - start;
    Logging::write(
      Info,
      "Audio::Mixer::getStreamSize",
      "Found stream size of " + std::to_string(*size)
    );

    return OK;
  }
};

} // Vst3
} // Effects
} // Audio
} // Gj


#endif //GJAUDIOVST3UTIL_H
