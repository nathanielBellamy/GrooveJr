//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "AudioFileQueryModel.h"
#include "../../../AppState.h"
#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class AudioFileTableView final : public QTableView {

  MusicLibraryFilters* filters;
  AudioFileQueryModel* audioFileQueryModel;

  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    AudioFileTableView(QWidget* parent, MusicLibraryFilters* filters)
        : QTableView(parent)
        , filters(filters)
        , audioFileQueryModel(new AudioFileQueryModel(this, filters)) {
      setModel(audioFileQueryModel);
      setStyle();
    };

    ~AudioFileTableView() {
      delete audioFileQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      audioFileQueryModel->hydrateState(appStatePacket);
      return OK;
    };

    Result refresh() {
      audioFileQueryModel->refresh();
      return OK;
    }
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
