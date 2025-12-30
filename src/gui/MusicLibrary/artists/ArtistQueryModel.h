//
// Created by ns on 10/11/25.
//

#ifndef ARTISTQUERYMODEL_H
#define ARTISTQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../state/Core.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"
#include "../../QSql/SqlWorkerPool.h"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {
class ArtistQueryModel final : public MusicLibraryQueryModel {
public:
  explicit ArtistQueryModel(QObject* parent, State::Core* stateCore, MusicLibraryFilters* filters,
                            SqlWorkerPool* sqlWorkerPool)
  : MusicLibraryQueryModel(parent, stateCore, filters, ARTIST, QString("ArtistQueryModel"), sqlWorkerPool) {
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

#endif //ARTISTQUERYMODEL_H
