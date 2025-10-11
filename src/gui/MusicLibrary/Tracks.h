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

namespace Gj {
namespace Gui {

class Tracks final : public QSqlQueryModel {

public:
  Tracks(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {}

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKS_H
