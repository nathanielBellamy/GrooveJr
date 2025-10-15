//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../MusicLibraryFilters.h"
#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class AudioFileQueryModel final : public QSqlQueryModel {
  MusicLibraryFilters* filters;
  QString query;

public:
  explicit AudioFileQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : QSqlQueryModel(parent)
    , filters(filters)
    {
    refresh();
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex &item, int role) const override;

  Result refresh();
};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
