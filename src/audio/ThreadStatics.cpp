//
// Created by ns on 11/24/24.
//

#include "ThreadStatics.h"

namespace Gj {
namespace Audio {

const char* ThreadStatics::filePath = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/06 Love Is A Losing Game.flac";
std::mutex ThreadStatics::filePathMutex;

sf_count_t ThreadStatics::frameId = 0;
std::mutex ThreadStatics::frameIdMutex;

std::atomic<bool> ThreadStatics::userSettingFrameId { false };
std::atomic<sf_count_t> ThreadStatics::readCount { 0 };

PlayState ThreadStatics::playState = STOP;
std::mutex ThreadStatics::playStateMutex;

float ThreadStatics::playbackSpeed = 0.f;
std::mutex ThreadStatics::playbackSpeedMutex;

bool ThreadStatics::readComplete = false;
std::mutex ThreadStatics::readCompleteMutex;

long ThreadStatics::threadId = 0;
std::mutex ThreadStatics::threadIdMutex;

const char* ThreadStatics::getFilePath() {
  std::lock_guard guard(filePathMutex);
  return filePath;
}

void ThreadStatics::setFilePath(const char* newFilePath) {
  std::lock_guard guard(filePathMutex);
  filePath = newFilePath;
}

sf_count_t ThreadStatics::getFrameId() {
  std::lock_guard guard(frameIdMutex);
  return frameId;
}

void ThreadStatics::setFrameId(sf_count_t newId) {
  std::lock_guard guard(frameIdMutex);
  frameId = newId;
}

void ThreadStatics::setPlaybackSpeed(float newSpeed) {
  std::lock_guard guard(playbackSpeedMutex);
  playbackSpeed = newSpeed;
}

float ThreadStatics::getPlaybackSpeed() {
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
  if (playState == PLAY)
    setReadComplete(false);
  playState = newState;
}

bool ThreadStatics::getReadComplete() {
  std::lock_guard guard(readCompleteMutex);
  return readComplete;
}

void ThreadStatics::setReadComplete(bool val) {
  std::lock_guard guard(readCompleteMutex);
  readComplete = val;
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

sf_count_t ThreadStatics::getReadCount() {
  return readCount.load();
}

void ThreadStatics::setReadCount(const sf_count_t newReadCount) {
  readCount.store(newReadCount);
}

void ThreadStatics::setUserSettingFrameId(const bool newUserSettingFrameId) {
  userSettingFrameId.store(newUserSettingFrameId);
}

bool ThreadStatics::getUserSettingFrameId() {
  return userSettingFrameId.load();
}

} // Audio
} // Gj