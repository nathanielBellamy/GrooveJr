//
// Created by ns on 1/4/25.
//

#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include <string>

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

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
             std::cout << "EffectsManager : add_effect_a " << channel << " " << path << std::endl;
//             strong_actor_ptr appStateManager = self->system().registry().get(APP_STATE_MANAGER);
           },
           [this](strong_actor_ptr replyToPtr, int channel, int effectIndex, remove_effect_a) {
             std::cout << "EffectsManager : current_state_a " << std::endl;
           },
       };
     };
};

} // Act
} // Gj


#endif //EFFECTSMANAGER_H
