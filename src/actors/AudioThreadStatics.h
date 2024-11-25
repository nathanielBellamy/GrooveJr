//
// Created by ns on 11/24/24.
//

#ifndef AUDIOTHREADSTATICS_H
#define AUDIOTHREADSTATICS_H

namespace Gj {
namespace Act {

class AudioThreadStatics {
  public:
    static long threadId;
    static long frameId;
    static int playState;
    static float playbackSpeed;
    static bool readComplete;

    static void setFrameId(long frameId);
    static void setPlayState(int playState);
    static void setPlaybackSpeed(float playbackSpeed);
    static void setReadComplete(bool readComplete);

    static void threadIdIncr();
};

} // Act
} // Gj


#endif //AUDIOTHREADSTATICS_H
