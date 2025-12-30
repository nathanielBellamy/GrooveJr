//
// Created by ns on 10/11/25.
//

#ifndef CACHEQUERYMODEL_H
#define CACHEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../QSql/SqlWorkerPool.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"

#include "../../../state/Core.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {
class CacheQueryModel final : public MusicLibraryQueryModel {
  bool isSelected(const QModelIndex& item) const;

public:
  explicit CacheQueryModel(QObject* parent, State::Core* stateCore, MusicLibraryFilters* filters,
                           SqlWorkerPool* sqlWorkerPool)
  : MusicLibraryQueryModel(parent, stateCore, filters, CACHE, QString("Cache"), sqlWorkerPool) {
    refresh();
  }

  Result hydrateState(const State::Packet& statePacket) override;

  Result refresh(bool hard = false) override;

  Result setHeaders() override;

  bool isCurrentlyPlaying(const QModelIndex& item) const override;

  QVariant data(const QModelIndex& item, int role) const override;
};
} // Gui
} // Gj

#endif //CACHEQUERYMODEL_H
