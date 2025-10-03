//
// Created by ns on 11/24/24.
//

#ifndef THREADSTATICS_H
#define THREADSTATICS_H

#include <atomic>
#include <mutex>

#include <sndfile.h>

#include "Logging.h"
#include "../enums/PlayState.h"

namespace Gj {
namespace Audio {

class ThreadStatics {
  static const char* filePath;
  static sf_count_t frameId;
  static std::atomic<bool> userSettingFrameId;
  static std::atomic<sf_count_t> frames;
  static PlayState playState;
  static sf_count_t playbackSpeed;
  static long threadId;

public:
  static std::mutex filePathMutex;
  static void setFilePath(const char* path);
  static const char* getFilePath();

  static std::mutex frameIdMutex;
  static void setFrameId(sf_count_t frameId);
  static sf_count_t getFrameId();

  static std::mutex playbackSpeedMutex;
  static sf_count_t getPlaybackSpeed();
  static void setPlaybackSpeed(sf_count_t playbackSpeed);

  static std::mutex playStateMutex;
  static PlayState getPlayState();
  static void setPlayState(PlayState playState);

  static std::mutex threadIdMutex;
  static long incrThreadId();
  static long getThreadId();

  static void setUserSettingFrameId(bool newUserSettingFrameId);
  static bool getUserSettingFrameId();

  static sf_count_t getFrames();
  static void setFrames(sf_count_t readCount);
};

} // Act
} // Gj


#endif //AUDIOTHREADSTATICS_H
