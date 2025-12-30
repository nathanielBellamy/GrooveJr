//
// Created by ns on 10/10/25.
//

#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <string>
#include <map>

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/event_based_actor.hpp"

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Playback.h"
#include "../state/Core.h"

#include "../db/Dao.h"

#include "../scanner/Scanner.h"

using namespace caf;

namespace Gj {
namespace Act {
struct MusicLibraryManagerTrait {
  using signatures = type_list<
    result<void>(std::string /*dirPath*/, ml_scan_dir_a)
  >;
};

using MusicLibraryManager = typed_actor<MusicLibraryManagerTrait>;

struct MusicLibraryManagerState {
  MusicLibraryManager::pointer self;
  Audio::Mixer::Core* mixer;
  Db::Dao* dao;
  State::Core* stateCore;

  MusicLibraryManagerState(
    MusicLibraryManager::pointer self,
    strong_actor_ptr supervisor,
    Audio::Mixer::Core* mixer,
    State::Core* stateCore
  )
  : self(self)
    , mixer(mixer)
    , dao(mixer->dao)
    , stateCore(stateCore) {
    self->link_to(supervisor);
    self->system().registry().put(MUSIC_LIBRARY_MANAGER, actor_cast<strong_actor_ptr>(self));
  }

  MusicLibraryManager::behavior_type make_behavior() {
    return {
      [this](std::string dirPath, ml_scan_dir_a) {
        Logging::write(
          Info,
          "Act::MusicLibraryManager::ml_scan_dir_a",
          "Scanning dirPath  " + dirPath
        );

        try {
          const Scanner::Scanner scanner(dao);
          scanner.scanDirectoryRecursive(dirPath);
          const auto appStateManagerPtr = self->system().registry().get(APP_STATE_MANAGER);
          self->anon_send(
            actor_cast<actor>(appStateManagerPtr),
            hydrate_display_a_v
          );
        } catch (...) {
          Logging::write(
            Error,
            "Act::MusicLibraryManager::ml_scan_dir_a",
            "An error occurred while scanning dirPath " + dirPath
          );
        }
      }
    };
  }
};
} // Act
} // Gj

#endif //MUSICLIBRARYMANAGER_H
