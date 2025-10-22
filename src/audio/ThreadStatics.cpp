//
// Created by ns on 11/24/24.
//

#include "ThreadStatics.h"

namespace Gj {
namespace Audio {

sf_count_t ThreadStatics::frameId = 0;
std::mutex ThreadStatics::frameIdMutex;

std::atomic<bool> ThreadStatics::userSettingFrameId { false };
std::atomic<sf_count_t> ThreadStatics::frames { 0 };

PlayState ThreadStatics::playState = STOP;
std::mutex ThreadStatics::playStateMutex;

sf_count_t ThreadStatics::playbackSpeed = 100LL;
std::mutex ThreadStatics::playbackSpeedMutex;

long ThreadStatics::threadId = 0;
std::mutex ThreadStatics::threadIdMutex;

sf_count_t ThreadStatics::getFrameId() {
  std::lock_guard guard(frameIdMutex);
  return frameId;
}

void ThreadStatics::setFrameId(sf_count_t newId) {
  std::lock_guard guard(frameIdMutex);
  frameId = newId;
}

void ThreadStatics::setPlaybackSpeed(const sf_count_t newSpeed) {
  std::lock_guard guard(playbackSpeedMutex);
  playbackSpeed = newSpeed;
}

sf_count_t ThreadStatics::getPlaybackSpeed() {
  std::lock_guard guard(playbackSpeedMutex);
  return playbackSpeed;
}

Gj::PlayState ThreadStatics::getPlayState() {
  std::lock_guard guard(playStateMutex);
  return playState;
}

void ThreadStatics::setPlayState(PlayState newState) {
  std::lock_guard guard(playStateMutex);
  Logging::write(
    Info,
    "Audio::ThreadStatics::setPlayState",
    "New state: " + std::to_string(newState)
  );

  if (playState == STOP)
    setFrameId(0);
  playState = newState;
}

long ThreadStatics::incrThreadId() {
  std::lock_guard guard(threadIdMutex);
  Logging::write(
    Info,
    "Audio::ThreadStatics::incrThreadId",
    "Old ThreadId: " + std::to_string(threadId)
  );
  threadId += 1;
  return threadId;
}

long ThreadStatics::getThreadId() {
  std::lock_guard guard(threadIdMutex);
  return threadId;
}

void ThreadStatics::setUserSettingFrameId(const bool newUserSettingFrameId) {
  userSettingFrameId.store(newUserSettingFrameId);
}

bool ThreadStatics::getUserSettingFrameId() {
  return userSettingFrameId.load();
}

sf_count_t ThreadStatics::getFrames() {
  return frames.load();
}

void ThreadStatics::setFrames(const sf_count_t newFrames) {
  frames.store(newFrames);
}

} // Audio
} // Gj