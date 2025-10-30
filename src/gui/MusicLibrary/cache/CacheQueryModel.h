//
// Created by ns on 10/11/25.
//

#ifndef CACHEQUERYMODEL_H
#define CACHEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class CacheQueryModel final : public MusicLibraryQueryModel {
  bool isSelected(const QModelIndex& item) const;

public:
  explicit CacheQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, gAppState, filters, CACHE)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //CACHEQUERYMODEL_H
