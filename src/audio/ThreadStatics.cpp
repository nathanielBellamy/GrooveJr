//
// Created by ns on 11/24/24.
//

#include "ThreadStatics.h"

namespace Gj {
namespace Audio {

const char* ThreadStatics::filePath = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/06 Love Is A Losing Game.flac";
std::mutex ThreadStatics::filePathMutex;

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

const char* ThreadStatics::getFilePath() {
  std::lock_guard<std::mutex> guard(filePathMutex);
  return filePath;
}

void ThreadStatics::setFilePath(const char* newFilePath) {
  std::lock_guard<std::mutex> guard(filePathMutex);
  filePath = newFilePath;
}

long ThreadStatics::getFrameId() {
  std::cout << "ThreadStatics : getFrameId" << std::endl;
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
  std::cout << "ThreadStatics : setPlayState : " << newState << std::endl;
  if (playState == Gj::PlayState::STOP)
    setFrameId(0);
  if (playState == Gj::PlayState::PLAY)
    setReadComplete(false);
  playState = newState;
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
  std::cout << "ThreadStatics : incrThreadId" << std::endl;
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