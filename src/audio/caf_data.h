#ifndef CAF_DATA_H
#define CAF_DATA_H

#include "caf/actor_system.hpp"

// TODO:
// - this struct is a development place holder as we replace JNI_DATA

using namespace caf;

struct CAF_DATA{
    actor_system& actorSystem;
    long gsPlayback;
    long getThreadId;
    long setCurrFrameId;
    long getPlayStateInt;
    long setPlayStateInt;
    long getPlaybackSpeedFloat;
    long setReadComplete;
    long jLong;
    long jLongInit;
    long jInteger;
    long jIntegerInit;

    CAF_DATA(caf::actor_system& actorSystem) :
        actorSystem(actorSystem)
          , gsPlayback(0)
          , getThreadId(0)
          , setCurrFrameId(0)
          , getPlayStateInt(0)
          , setPlayStateInt(0)
          , getPlaybackSpeedFloat(0)
          , setReadComplete(0)
          , jLong(0)
          , jLongInit(0)
          , jInteger(0)
          , jIntegerInit(0) {}
};

#endif
