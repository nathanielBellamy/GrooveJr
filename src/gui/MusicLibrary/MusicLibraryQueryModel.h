//
// Created by ns on 10/16/25.
//

#ifndef MUSICLIBRARYQUERYMODEL_H
#define MUSICLIBRARYQUERYMODEL_H

#include <vector>

#include <QObject>
#include <QtSql/qsqlquerymodel.h>
#include <QStandardItemModel>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../AppState.h"
#include "../../enums/Result.h"

#include "../QSql/SqlWorkerPool.h"
#include "../QSql/SqlWorkerPoolClient.h"
#include "MusicLibraryFilters.h"
#include "Constants.h"

namespace Gj {
namespace Gui {
  class SqlWorkerPool;

  class MusicLibraryQueryModel : public SqlWorkerPoolClient {
  protected:
    QString id;
    SqlWorkerPool* sqlWorkerPool;
    AppState* gAppState;
    MusicLibraryType type;
    MusicLibraryFilters* filters;
    QString queryString;

    Result setQueryString(const std::string& newQueryString) {
      queryString = QString(newQueryString.c_str());
      // setQuery(queryString);
      return OK;
    };

    bool isCurrentFilter(const QModelIndex& item, const size_t idCol) const {
      std::vector<Db::ID> ids = filters->filters.at(type).ids;
      return std::find(
          ids.begin(),
          ids.end(),
          index(item.row(), idCol).data()
          ) != ids.end();
    };

    bool isCurrentlyPlaying(const QModelIndex& item, const size_t idCol) const {
      const auto currentlyPlaying = gAppState->getCurrentlyPlaying();
      Db::ID id;
      switch (type) {
        case ALBUM:
          id = index(item.row(), ALBUM_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().album.id == id;
        case ARTIST:
          id = index(item.row(), ARTIST_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().album.id == id;
        case AUDIO_FILE:
          id = index(item.row(), AUDIO_FILE_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().audioFile.id == id;
        case CACHE:
          id = index(item.row(), AUDIO_FILE_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().audioFile.id == id && !gAppState->queuePlay;
        case QUEUE:
          id = index(item.row(), AUDIO_FILE_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().audioFile.id == id && gAppState->queuePlay;
        case GENRE:
          id = index(item.row(), GENRE_COL_ID).data().toULongLong();
          return gAppState->getCurrentlyPlaying().genre.id == id;
        default:
          return false;
      }
    }

  public:
    MusicLibraryQueryModel(
      QObject* parent,
      AppState* gAppState,
      MusicLibraryFilters* filters,
      const MusicLibraryType type,
      const QString& id,
      SqlWorkerPool* sqlWorkerPool
      )
      : SqlWorkerPoolClient(parent)
      , id(id)
      , sqlWorkerPool(sqlWorkerPool)
      , gAppState(gAppState)
      , type(type)
      , filters(filters)
    {}

    virtual Result hydrateState(const AppStatePacket& appStatePacket) = 0;
    virtual Result refresh() = 0;
};

} // Gui
} // Gj

#endif //MUSICLIBRARYQUERYMODEL_H
