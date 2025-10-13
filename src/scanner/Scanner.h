//
// Created by ns on 10/4/25.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <filesystem>
#include <map>
#include <vector>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "../db/Dao.h"
#include "../db/Types.h"
#include "../db/dto/musicLibrary/AlbumWithArtist.h"
#include "../db/dto/musicLibrary/GenreWithTrackId.h"
#include "../db/dto/musicLibrary/TrackDecoratedDto.h"
#include "../db/entity/musicLibrary/Album.h"
#include "../db/entity/musicLibrary/Artist.h"
#include "../db/entity/musicLibrary/AudioFile.h"
#include "../db/entity/musicLibrary/Genre.h"
#include "../db/entity/musicLibrary/Track.h"
#include "../db/entity/musicLibrary/Playlist.h"

#include "./Logging.h"
#include "../enums/Result.h"

namespace Gj {
namespace Scanner {

class Scanner final {

  Db::Dao* dao;

  public:
    Scanner(Db::Dao* dao);

    Result scanDirectoryRecursive(const std::string& dirPath) const;
};

} // Sc
} // Gj


#endif //SCANNER_H
