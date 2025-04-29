
#ifndef ATOMS_H
#define ATOMS_H

#include "../AppState.h"
#include "./EnvelopeQtPtr.h"

CAF_BEGIN_TYPE_ID_BLOCK(groovejr, first_custom_type_id)

  // typeIds
  CAF_ADD_TYPE_ID(groovejr, (Gj::AppStatePacket))
  CAF_ADD_TYPE_ID(groovejr, (Gj::EnvelopeQtPtr))

  // atoms
  CAF_ADD_ATOM(groovejr, supervisor_status_a)

  CAF_ADD_ATOM(groovejr, current_state_a)
  CAF_ADD_ATOM(groovejr, hydrate_display_a)
  CAF_ADD_ATOM(groovejr, read_state_a)

  // Mixer
  CAF_ADD_ATOM(groovejr, mix_add_effects_channel_a)
  CAF_ADD_ATOM(groovejr, mix_add_effect_to_channel_a)

  // Transport Control
  CAF_ADD_ATOM(groovejr, tc_trig_play_a)
  CAF_ADD_ATOM(groovejr, tc_trig_play_ar)
  CAF_ADD_ATOM(groovejr, tc_trig_pause_a)
  CAF_ADD_ATOM(groovejr, tc_trig_pause_ar)
  CAF_ADD_ATOM(groovejr, tc_trig_stop_a)
  CAF_ADD_ATOM(groovejr, tc_trig_stop_ar)
  CAF_ADD_ATOM(groovejr, tc_trig_rw_a)
  CAF_ADD_ATOM(groovejr, tc_trig_rw_ar)
  CAF_ADD_ATOM(groovejr, tc_trig_ff_a)
  CAF_ADD_ATOM(groovejr, tc_trig_ff_ar)

  CAF_ADD_ATOM(groovejr, audio_thread_init_a)

  CAF_ADD_ATOM(groovejr, add_effect_a)
  CAF_ADD_ATOM(groovejr, remove_effect_a)

CAF_END_TYPE_ID_BLOCK(groovejr)

#endif //MAIN_H
