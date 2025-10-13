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

namespace Gj {
namespace Gui {

class PlaylistQueryModel final : public QSqlQueryModel {
  QString query = QString("select name from playlists");

public:
  explicit PlaylistQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //PLAYLISTQUERYMODEL_H
