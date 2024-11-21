
#ifndef ATOMS_H
#define ATOMS_H

#include "../AppState.h"
#include "./EnvelopeQtPtr.h"

CAF_BEGIN_TYPE_ID_BLOCK(groovesprings, first_custom_type_id)

  // typeIds
  CAF_ADD_TYPE_ID(groovesprings, (Gs::AppStatePacket))
  CAF_ADD_TYPE_ID(groovesprings, (Gs::EnvelopeQtPtr))

  // atoms
  CAF_ADD_ATOM(groovesprings, supervisor_status_a)

  CAF_ADD_ATOM(groovesprings, current_state_a)
  CAF_ADD_ATOM(groovesprings, hydrate_display_a)
  CAF_ADD_ATOM(groovesprings, read_state_a)

  CAF_ADD_ATOM(groovesprings, tc_trig_play_a)
  CAF_ADD_ATOM(groovesprings, tc_trig_play_ar)
  CAF_ADD_ATOM(groovesprings, tc_trig_pause_a)
  CAF_ADD_ATOM(groovesprings, tc_trig_pause_ar)
  CAF_ADD_ATOM(groovesprings, tc_trig_stop_a)
  CAF_ADD_ATOM(groovesprings, tc_trig_stop_ar)
  CAF_ADD_ATOM(groovesprings, tc_trig_rw_a)
  CAF_ADD_ATOM(groovesprings, tc_trig_rw_ar)
  CAF_ADD_ATOM(groovesprings, tc_trig_ff_a)
  CAF_ADD_ATOM(groovesprings, tc_trig_ff_ar)

CAF_END_TYPE_ID_BLOCK(groovesprings)

#endif //MAIN_H
