//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

constexpr size_t AUDIO_FILE_COL_TRACK = 0;
constexpr size_t AUDIO_FILE_COL_ARTIST = 1;
constexpr size_t AUDIO_FILE_COL_ALBUM = 2;
constexpr size_t AUDIO_FILE_COL_TRACK_NUMBER = 3;
constexpr size_t AUDIO_FILE_COL_YEAR = 4;
constexpr size_t AUDIO_FILE_COL_GENRE = 5;
constexpr size_t AUDIO_FILE_COL_PATH = 6;
constexpr size_t AUDIO_FILE_COL_ID = 7;

class AudioFileQueryModel final : public MusicLibraryQueryModel {

public:
  explicit AudioFileQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, AUDIO_FILE)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
