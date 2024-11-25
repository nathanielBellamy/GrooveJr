#ifndef CAF_DATA_H
#define CAF_DATA_H

#include "caf/actor_system.hpp"

// TODO:
// - this struct is a development place holder as we replace JNI_DATA

using namespace caf;

struct CAF_DATA{
    actor_system& actorSystem;

    CAF_DATA(caf::actor_system& actorSystem) :
        actorSystem(actorSystem)
        {}
};

#endif
