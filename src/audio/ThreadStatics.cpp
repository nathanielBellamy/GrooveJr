//
// Created by ns on 11/24/24.
//

#include "ThreadStatics.h"

namespace Gj {
namespace Audio {

long ThreadStatics::frameId = 0;
std::mutex ThreadStatics::frameIdMutex;

Gj::PlayState ThreadStatics::playState = Gj::PlayState::STOP;
std::mutex ThreadStatics::playStateMutex;

float ThreadStatics::playbackSpeed = 0.f;
std::mutex ThreadStatics::playbackSpeedMutex;

bool ThreadStatics::readComplete = false;
std::mutex ThreadStatics::readCompleteMutex;

long ThreadStatics::threadId = 0;
std::mutex ThreadStatics::threadIdMutex;

long ThreadStatics::getFrameId() {
  std::lock_guard<std::mutex> guard(frameIdMutex);
  return frameId;
}

void ThreadStatics::setFrameId(long newId) {
  std::lock_guard<std::mutex> guard(frameIdMutex);
  frameId = newId;
}

void ThreadStatics::setPlaybackSpeed(float newSpeed) {
  std::lock_guard<std::mutex> guard(playbackSpeedMutex);
  playbackSpeed = newSpeed;
}

float ThreadStatics::getPlaybackSpeed() {
  std::lock_guard<std::mutex> guard(playbackSpeedMutex);
  return playbackSpeed;
}

Gj::PlayState ThreadStatics::getPlayState() {
  std::lock_guard<std::mutex> guard(playStateMutex);
  return playState;
}

void ThreadStatics::setPlayState(Gj::PlayState newState) {
  std::lock_guard<std::mutex> guard(playStateMutex);
  playState = newState;
}
void ThreadStatics::setPlayState(int newState) {
  std::lock_guard<std::mutex> guard(playStateMutex);
  playState = Gj::intToPs(newState);
}

bool ThreadStatics::getReadComplete() {
  std::lock_guard<std::mutex> guard(readCompleteMutex);
  return readComplete;
}

void ThreadStatics::setReadComplete(bool val) {
  std::lock_guard<std::mutex> guard(readCompleteMutex);
  readComplete = val;
}

long ThreadStatics::incrThreadId() {
  std::lock_guard<std::mutex> guard(threadIdMutex);
  threadId += 1;
  return threadId;
}

long ThreadStatics::getThreadId() {
  std::lock_guard<std::mutex> guard(threadIdMutex);
  return ThreadStatics::threadId;
}

} // Audio
} // Gj