//
// Created by ns on 12/27/25.
//

#ifndef PROCESSDATACHANGEFLAG_H
#define PROCESSDATACHANGEFLAG_H

#include <sndfile.h>

namespace Gj {
namespace Audio {
typedef sf_count_t ProcessDataChangeFlag_t;

namespace ProcessDataChangeFlag {
static constexpr sf_count_t BASE = 0;
static constexpr sf_count_t ACKNOWLEDGE = 1;
static constexpr sf_count_t ROGER = 2;
}
} // Audio
} // Gj

#endif //PROCESSDATACHANGEFLAG_H
