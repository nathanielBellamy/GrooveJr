//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILEQUERYMODEL_H
#define AUDIOFILEQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QThread>

#include "../../QSql/SqlWorkerPool.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class AudioFileQueryModel final : public MusicLibraryQueryModel {

public:
  explicit AudioFileQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters, SqlWorkerPool* sqlWorkerPool)
    : MusicLibraryQueryModel(parent, gAppState, filters, AUDIO_FILE, QString("AudioFileQueryModel"), sqlWorkerPool)
    {
    refresh();
  }
  QVariant data(const QModelIndex& item, int role) const override;

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;
  Result setHeaders() override;

  void initSqlWorkerPool();
};

} // Gui
} // Gj

#endif //AUDIOFILEQUERYMODEL_H
