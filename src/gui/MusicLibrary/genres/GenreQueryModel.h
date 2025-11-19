//
// Created by ns on 10/11/25.
//

#ifndef GENREQUERYMODEL_H
#define GENREQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../AppState.h"
#include "../../../enums/Result.h"
#include "../../../Logging.h"
#include "../../QSql/SqlWorkerPool.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class GenreQueryModel final : public MusicLibraryQueryModel {

public:
  explicit GenreQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters, SqlWorkerPool* sqlWorkerPool)
    : MusicLibraryQueryModel(parent, gAppState, filters, GENRE, QString("GenreQueryModel"), sqlWorkerPool)
    {
    refresh();
  }

  Result refresh(bool hard = false) override;
  Result hydrateState(const AppStatePacket &appStatePacket) override;
  Result setHeaders() override;
  bool isCurrentlyPlaying(const QModelIndex& item) const override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //GENREQUERYMODEL_H
