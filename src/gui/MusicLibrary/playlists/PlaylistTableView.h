//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "PlaylistQueryModel.h"
#include "../../../AppState.h"
#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public QTableView {
  PlaylistQueryModel* playlistQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    PlaylistTableView(QWidget* parent, MusicLibraryFilters* filters)
        : QTableView(parent)
        , playlistQueryModel(new PlaylistQueryModel(this, filters)) {
      setModel(playlistQueryModel);
      setStyle();
    };

    ~PlaylistTableView() {
      delete playlistQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      playlistQueryModel->hydrateState(appStatePacket);
      return OK;
    };

    Result refresh() {
      playlistQueryModel->refresh();
      return OK;
    };
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
