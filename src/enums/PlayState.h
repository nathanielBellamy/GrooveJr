//
// Created by ns on 11/15/24.
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H


namespace Gj {

    enum PlayState {
      STOP,
      PLAY,
      PAUSE,
      RW,
      FF
    };

    static int psToInt(const PlayState playState) { return static_cast<int>(playState); };
    static PlayState intToPs(const int playStateInt) {
      if (playStateInt == 0) {
        return STOP;
      }
      if (playStateInt == 1) {
        return PLAY;
      }
      if (playStateInt == 2) {
        return PAUSE;
      }
      if (playStateInt == 3) {
        return RW;
      }
      if (playStateInt == 4) {
        return FF;
      }

      return STOP;
    };

} // Gj
#endif //PLAYSTATE_H
