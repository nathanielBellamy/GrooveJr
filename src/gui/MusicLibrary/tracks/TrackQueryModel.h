//
// Created by ns on 10/11/25.
//

#ifndef TRACKQUERYMODEL_H
#define TRACKQUERYMODEL_H

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

class TrackQueryModel final : public QSqlQueryModel {
  QString query = QString("select title, trackNumber from tracks");
  MusicLibraryFilters* filters;

public:
  explicit TrackQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : QSqlQueryModel(parent)
    , filters(filters)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket);

  Result refresh() {
    if (!filters->albumIds.empty()) {
      const std::string str = "select title, trackNumber from tracks where albumId = " + std::to_string(filters->albumIds.front());
      query = QString(str.c_str());
    }

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("TrackNumber"));

    return OK;
  }

  QVariant data(const QModelIndex& index, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKQUERYMODEL_H
