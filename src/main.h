
#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <chrono>
#include <string>
#include <thread>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "./messaging/atoms.h"
#include "./actors/Supervisor.h"

#include <QApplication>
#include "./gui/MainWindow.h"

#include "./audio/audio.h"
#include "./audio/effects/vst3/host/audiohost/source/audiohost.h"
#include "./audio/effects/vst3/host/hostclasses.hpp"
#include "./audio/effects/vst3/host/editorhost/source/editorhost.h"

namespace Gs {

extern "C" {
    int main(int argc, char *argv[]);
}

} // Gs

#endif //MAIN_H
