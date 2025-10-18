//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTQUERYMODEL_H
#define PLAYLISTQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>

#include "../../../AppState.h"
#include "../../../enums/Result.h"
#include "../../../Logging.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryQueryModel.h"

namespace Gj {
namespace Gui {

constexpr size_t PLAYLIST_COL_NAME = 0;
constexpr size_t PLAYLIST_COL_ID = 1;

class PlaylistQueryModel final : public MusicLibraryQueryModel {

public:
  explicit PlaylistQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : MusicLibraryQueryModel(parent, filters, PLAYLIST)
    {
    refresh();
    std::cout << "playlist query model" << std::endl;
  }

  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //PLAYLISTQUERYMODEL_H
