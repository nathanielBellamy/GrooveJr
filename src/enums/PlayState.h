//
// Created by ns on 11/15/24.
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H


namespace Gs {

    enum PlayState {
      STOP,
      PLAY,
      PAUSE,
      RW,
      FF
    };

    static int psToInt(PlayState playState) { return static_cast<int>(playState); };
    static PlayState intToPs(int playStateInt) {
      if (playStateInt == 0) {
        return PlayState::STOP;
      } else if (playStateInt == 1) {
        return PlayState::PLAY;
      } else if (playStateInt == 2) {
        return PlayState::PAUSE;
      } else if (playStateInt == 3) {
        return PlayState::RW;
      } else if (playStateInt == 4) {
        return PlayState::FF;
      } else {
        return PlayState::STOP;
      }
    };

} // Gs
#endif //PLAYSTATE_H
