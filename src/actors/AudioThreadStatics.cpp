//
// Created by ns on 11/24/24.
//

#include "AudioThreadStatics.h"

namespace Gj {
namespace Act {

long AudioThreadStatics::threadId = 0l;
long AudioThreadStatics::frameId = 0l;
int AudioThreadStatics::playState = 0;
float AudioThreadStatics::playbackSpeed = 0.f;
bool AudioThreadStatics::readComplete = false;

void AudioThreadStatics::setFrameId(long frameId) {
  AudioThreadStatics::frameId = frameId;
}

void AudioThreadStatics::setPlayState(int newState) {
  AudioThreadStatics::playState = newState;
}

void AudioThreadStatics::setPlaybackSpeed(float newSpeed) {
  AudioThreadStatics::playbackSpeed = newSpeed;
}

void AudioThreadStatics::setReadComplete(bool readComplete) {
  AudioThreadStatics::readComplete = readComplete;
}

void AudioThreadStatics::threadIdIncr() {
  AudioThreadStatics::threadId++;
}

} // Act
} // Gj