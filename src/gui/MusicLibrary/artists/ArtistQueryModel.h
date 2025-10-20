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

namespace Gj {
namespace Gui {

constexpr size_t ARTIST_COL_NAME = 0;
constexpr size_t ARTIST_COL_ID = 1;

class ArtistQueryModel final : public MusicLibraryQueryModel {

  public:
  explicit ArtistQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, ARTIST)
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
