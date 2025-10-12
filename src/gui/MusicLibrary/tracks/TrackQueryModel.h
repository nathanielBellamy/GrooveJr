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

#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class TrackQueryModel final : public QSqlQueryModel {

public:
  explicit TrackQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery("select * from tracks");
    setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Album ID"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Title"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Track Number"));
  }

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKS_H
