//
// Created by ns on 10/11/25.
//

#ifndef TRACKS_H
#define TRACKS_H

#include <QtSql/qsqlquerymodel.h>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlError>

namespace Gj {
namespace Gui {

class Tracks final : public QSqlQueryModel {

public:
  Tracks(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {}
};

} // Gui
} // Gj

#endif //TRACKS_H
