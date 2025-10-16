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

class PlaylistQueryModel final : public QSqlQueryModel {
  MusicLibraryFilters* filters;
  QString query;

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
