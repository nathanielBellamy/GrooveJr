//
// Created by ns on 10/30/25.
//

#ifndef GJGUICONSTANTS_H
#define GJGUICONSTANTS_H

#include <cstddef>
#include <QColor>

namespace Gj {
namespace Gui {

constexpr size_t ALBUM_COL_TITLE = 0;
constexpr size_t ALBUM_COL_YEAR = 1;
constexpr size_t ALBUM_COL_ID = 2;

constexpr size_t ARTIST_COL_NAME = 0;
constexpr size_t ARTIST_COL_ID = 1;

constexpr size_t AUDIO_FILE_COL_TRACK = 0;
constexpr size_t AUDIO_FILE_COL_ARTIST = 1;
constexpr size_t AUDIO_FILE_COL_ALBUM = 2;
constexpr size_t AUDIO_FILE_COL_TRACK_NUMBER = 3;
constexpr size_t AUDIO_FILE_COL_YEAR = 4;
constexpr size_t AUDIO_FILE_COL_GENRE = 5;
constexpr size_t AUDIO_FILE_COL_PATH = 6;
constexpr size_t AUDIO_FILE_COL_ID = 7;

constexpr size_t GENRE_COL_NAME = 0;
constexpr size_t GENRE_COL_ID = 1;

constexpr size_t PLAYLIST_COL_NAME = 0;
constexpr size_t PLAYLIST_COL_ID = 1;

constexpr auto CURRENTLY_PLAYING_COLOR = Qt::blue;

}
}

#endif //GJGUICONSTANTS_H
