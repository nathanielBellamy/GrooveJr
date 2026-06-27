//
// Created by ns on 10/6/25.
//

#ifndef TRACKDECORATE_H
#define TRACKDECORATE_H

#include <sstream>
#include <sndfile.h>
#include <sqlite3.h>

#include "../../../types/Types.h"
#include "../../Logging.h"
#include "../../../enums/AudioCodec.h"
#include "../../../enums/Result.h"

#include "../../entity/musicLibrary/Album.h"
#include "../../entity/musicLibrary/Artist.h"
#include "../../entity/musicLibrary/AudioFile.h"
#include "../../entity/musicLibrary/Genre.h"
#include "../../entity/musicLibrary/Track.h"

namespace Gj {
namespace Db {

struct DecoratedAudioFile final {
  DecoratedAudioFile()
    : album(Album(" - "))
    , artist(Artist(" - "))
    , genre(" - ")
    {}


  DecoratedAudioFile(
    Album album,
    Artist artist,
    AudioFile audioFile,
    Genre genre,
    Track track
  );

  static DecoratedAudioFile deser(sqlite3_stmt* stmt);

  Album album;
  Artist artist;
  AudioFile audioFile;
  Genre genre;
  Track track;

  bool isValid() const {
    return audioFile.valid;
  }

  std::string toString() const {
    std::ostringstream stream;
    stream << std::boolalpha
      << "DecoratedAudioFile { "
      << "album: { "
      << "id: " << album.id
      << ", title: " << album.title.std_str()
      << ", year: " << album.year
      << " }, artist: { "
      << "id: " << artist.id
      << ", name: " << artist.name.std_str()
      << " }, audioFile: { "
      << "id: " << audioFile.id
      << ", trackId: " << audioFile.trackId
      << ", filePath: " << audioFile.filePath.std_str()
      << ", sfInfo: { "
      << "frames: " << audioFile.sfInfo.frames
      << ", samplerate: " << audioFile.sfInfo.samplerate
      << ", channels: " << audioFile.sfInfo.channels
      << ", format: " << audioFile.sfInfo.format
      << ", sections: " << audioFile.sfInfo.sections
      << ", seekable: " << audioFile.sfInfo.seekable
      << " }, valid: " << audioFile.valid
      << " }, genre: { "
      << "id: " << genre.id
      << ", name: " << genre.name.std_str()
      << " }, track: { "
      << "id: " << track.id
      << ", albumId: " << track.albumId
      << ", title: " << track.title.std_str()
      << ", trackNumber: " << track.trackNumber
      << " } }";
    return stream.str();
  }
};

} // Db
} // Gj

#endif //TRACKDECORATE_H
