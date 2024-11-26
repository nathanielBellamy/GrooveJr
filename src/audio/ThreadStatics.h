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
    static long threadId;

  public:
    static long frameId;
    static int playState;
    static float playbackSpeed;
    static bool readComplete;

    static void setFrameId(long frameId);
    static void setPlayState(int playState);
    static void setPlaybackSpeed(float playbackSpeed);
    static void setReadComplete(bool readComplete);

    static std::mutex threadIdMutex;
    static long incrThreadId();
    static long getThreadId();
};

} // Act
} // Gj


#endif //AUDIOTHREADSTATICS_H
