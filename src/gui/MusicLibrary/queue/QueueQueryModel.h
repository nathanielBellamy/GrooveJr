//
// Created by ns on 10/11/25.
//

#ifndef QUEUEQUERYMODEL_H
#define QUEUEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../QSql/SqlWorkerPool.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"

#include "../../../state/AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../../QSql/SqlWorkerPool.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {
class QueueQueryModel final : public MusicLibraryQueryModel {
  bool isSelected(const QModelIndex& item) const;

public:
  explicit QueueQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters,
                           SqlWorkerPool* sqlWorkerPool)
  : MusicLibraryQueryModel(parent, gAppState, filters, AUDIO_FILE, QString("QueueQueryModel"), sqlWorkerPool) {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;

  Result refresh(bool hard = false) override;

  Result setHeaders() override;

  bool isCurrentlyPlaying(const QModelIndex& item) const override;

  QVariant data(const QModelIndex& item, int role) const override;
};
} // Gui
} // Gj

#endif //QUEUEQUERYMODEL_H
