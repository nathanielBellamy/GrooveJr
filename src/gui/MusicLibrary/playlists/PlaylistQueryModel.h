//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTQUERYMODEL_H
#define PLAYLISTQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

constexpr size_t PLAYLIST_COL_NAME = 0;
constexpr size_t PLAYLIST_COL_ID = 1;

class PlaylistQueryModel final : public QSqlQueryModel {
  MusicLibraryFilters* filters;
  QString query;
  bool isSelected(const QModelIndex& item) const;

public:
  explicit PlaylistQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : QSqlQueryModel(parent)
    , filters(filters)
    {

    refresh();
  }
  Result hydrateState(const AppStatePacket& appStatePacket);
  Result refresh();

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //PLAYLISTQUERYMODEL_H
