//
// Created by ns on 10/11/25.
//

#ifndef TRACKS_H
#define TRACKS_H

#include <QtSql/qsqlquerymodel.h>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlError>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class TrackQueryModel final : public QSqlQueryModel {
  QString query = QString("select title, trackNumber from tracks");

public:
  explicit TrackQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("TrackNumber"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKS_H
