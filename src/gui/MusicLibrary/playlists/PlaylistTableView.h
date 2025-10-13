//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "PlaylistQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public QTableView {
  PlaylistQueryModel* playlistQueryModel;

  public:
    PlaylistTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , playlistQueryModel(new PlaylistQueryModel(this)) {
      setModel(playlistQueryModel);
    };

    ~PlaylistTableView() {
      delete playlistQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      playlistQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
