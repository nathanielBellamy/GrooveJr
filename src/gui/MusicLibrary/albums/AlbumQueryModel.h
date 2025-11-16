//
// Created by ns on 10/11/25.
//

#ifndef ALBUMQUERYMODEL_H
#define ALBUMQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../../QSql/SqlWorkerPool.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class AlbumQueryModel final : public MusicLibraryQueryModel {

public:
  explicit AlbumQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters, SqlWorkerPool* sqlWorkerPool)
    : MusicLibraryQueryModel(parent, gAppState, filters, ALBUM, QString("AlbumQueryModel"), sqlWorkerPool)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;
  Result setHeaders() override;
  bool isCurrentlyPlaying(const QModelIndex& item) const override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //ALBUMQUERYMODEL_H
