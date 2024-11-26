//
// Created by ns on 11/24/24.
//

#include "ThreadStatics.h"

namespace Gj {
namespace Audio {

long ThreadStatics::threadId = 0;
long ThreadStatics::frameId = 0;
int ThreadStatics::playState = 0;
float ThreadStatics::playbackSpeed = 0.f;
bool ThreadStatics::readComplete = false;
std::mutex ThreadStatics::threadIdMutex;

void ThreadStatics::setFrameId(long newId) {
  frameId = newId;
}

void ThreadStatics::setPlayState(int newState) {
  playState = newState;
}

void ThreadStatics::setPlaybackSpeed(float newSpeed) {
  playbackSpeed = newSpeed;
}

void ThreadStatics::setReadComplete(bool val) {
  readComplete = val;
}

long ThreadStatics::incrThreadId() {
  std::lock_guard<std::mutex> guard(threadIdMutex);
  threadId += 1;
  std::cout << "Thread ID: " << threadId << std::endl;
  return threadId;
}

long ThreadStatics::getThreadId() {
  std::lock_guard<std::mutex> guard(threadIdMutex);
  return ThreadStatics::threadId;
}

} // Audio
} // Gj