//
// Created by ns on 10/4/25.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "../db/Dao.h"
#include "../db/entity/musicLibrary/Track.h"
#include "../db/repository/musicLibrary/TrackRepository.h"

#include "./Logging.h"
#include "../enums/Result.h"

namespace Gj {
namespace Scanner {

class Scanner final {

  Db::Dao* dao;

  public:
    Scanner(Db::Dao* dao);

    Result runScan();
};

} // Sc
} // Gj


#endif //SCANNER_H
