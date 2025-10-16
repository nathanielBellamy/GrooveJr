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

#include "../GjQueryModel.h"

namespace Gj {
namespace Gui {

constexpr size_t TRACK_COL_ID = 2;

class TrackQueryModel final : public GjQueryModel {

public:
  explicit TrackQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : GjQueryModel(parent, filters)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket);

  Result refresh();

  QVariant data(const QModelIndex& item, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKQUERYMODEL_H
