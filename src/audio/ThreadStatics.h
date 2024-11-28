//
// Created by ns on 11/24/24.
//

#ifndef THREADSTATICS_H
#define THREADSTATICS_H

#include <iostream>
#include <mutex>
#include <thread>

namespace Gj {
namespace Audio {

class ThreadStatics {
  private:
    static long frameId;
    static int playState;
    static float playbackSpeed;
    static bool readComplete;
    static long threadId;

  public:

    static std::mutex frameIdMutex;
    static void setFrameId(long frameId);
    static long getFrameId();

    static std::mutex playbackSpeedMutex;
    static float getPlaybackSpeed();
    static void setPlaybackSpeed(float playbackSpeed);

    static std::mutex playStateMutex;
    static int getPlayState();
    static void setPlayState(int playState);

    static std::mutex readCompleteMutex;
    static bool getReadComplete();
    static void setReadComplete(bool readComplete);

    static std::mutex threadIdMutex;
    static long incrThreadId();
    static long getThreadId();
};

} // Act
} // Gj


#endif //AUDIOTHREADSTATICS_H
