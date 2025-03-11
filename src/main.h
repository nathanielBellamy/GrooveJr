
#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <signal.h>
#include <unordered_map>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "./messaging/atoms.h"
#include "./actors/Supervisor.h"

#include <QApplication>
#include "./gui/MainWindow.h"

#include "./audio/effects/vst3/host/App.h"
#include "./audio/effects/vst3/Plugin.h"
#include "./audio/Mixer.h"

#include "./AppState.h"
#include "./enums/PlayState.h"

namespace Gj {

extern "C" {
    int main(int argc, char *argv[]);
}

} // Gj

#endif //MAIN_H
