//
// Created by ns on 10/11/25.
//

#ifndef ARTISTQUERYMODEL_H
#define ARTISTQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"
#include "../Constants.h"

namespace Gj {
namespace Gui {

class ArtistQueryModel final : public MusicLibraryQueryModel {

public:
  explicit ArtistQueryModel(QObject* parent, AppState* gAppState, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, gAppState, filters, ARTIST)
    {
    refresh();
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;

  QVariant data(const QModelIndex& index, int role) const override;
};

} // Gui
} // Gj

#endif //ARTISTQUERYMODEL_H
