//
// Created by ns on 1/4/25.
//

#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include <string>

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/event_based_actor.hpp"

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Playback.h"
#include "../AppState.h"

using namespace caf;

namespace Gj {
namespace Act {

struct EffectsManagerTrait {
    using signatures = type_list<
                                  result<void>(strong_actor_ptr,  int /*channel*/, std::string /*path*/, add_effect_a),
                                  result<void>(strong_actor_ptr,  int /*channel*/, int /*effect index*/, remove_effect_a)
                                >;
};

using EffectsManager = typed_actor<EffectsManagerTrait>;

struct EffectsManagerState {
     EffectsManager::pointer self;
     Audio::Mixer* mixer;

     EffectsManagerState(EffectsManager::pointer self, strong_actor_ptr supervisor, Audio::Mixer* mixer) :
         self(self)
       , mixer(mixer)
       {
           self->link_to(supervisor);
           self->system().registry().put(EFFECTS_MANAGER, actor_cast<strong_actor_ptr>(self));
       }

     EffectsManager::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr replyToPtr, int channel, std::string path, add_effect_a) {
             Logging::write(
               Info,
               "Act::EffectsManager::add_effect_a",
               "Adding effect " + path + " to channel " + std::to_string(channel)
             );

             try {
               strong_actor_ptr appStateManager = self->system().registry().get(APP_STATE_MANAGER);
               if (mixer->addEffectToChannel(channel, path.substr(7))) {
                 // mixer->showEditor(channel, index);
                 Logging::write(
                   Info,
                   "Act::EffectsManager::add_effect_a",
                   "Effect added " + path.substr(7) + " to channel " + std::to_string(channel)
                 );
               } else {
                 Logging::write(
                   Warning,
                   "Act::EffectsManager::add_effect_a",
                   "Failed to add effect " + path.substr(7)
                 );
               };
             } catch (...) {
               Logging::write(
                 Error,
                 "Act::EffectsManager::add_effect_a",
                 "An error occurred while adding effect " + path.substr(7) + " to channel " + std::to_string(channel)
               );
             }
           },
           [this](strong_actor_ptr replyToPtr, int channel, int effectIndex, remove_effect_a) {
             Logging::write(
               Error,
               "Act::EffectsManager::remove_effect_a",
               "Removing effect " + std::to_string(effectIndex) + " from channel " + std::to_string(channel)
             );
           },
       };
     };
};

} // Act
} // Gj

#endif //EFFECTSMANAGER_H
